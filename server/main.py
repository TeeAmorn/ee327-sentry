import base64
import os
from sched import scheduler
from cv2 import FONT_HERSHEY_SCRIPT_SIMPLEX
import numpy as np
import cv2
import socket
import time
import tornado.ioloop
import tornado.web
import tornado.websocket


SET_LOG = True
SET_DEBUG = True


def LOG(text):
    if SET_LOG:
        print(text)


def DEBUG(text):
    if SET_DEBUG:
        print(text)


def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP


class MainHandler(tornado.web.RequestHandler):

    def check_origin(self, origin): return True

    def get(self):
        self.render("home.html", ip=get_ip())


class StateManager:

    # WebSocket Connections
    sentry = None
    webpage = None
    cam0 = None
    stream0 = None
    cam1 = None
    stream1 = None
    cam2 = None
    stream2 = None
    cam3 = None
    stream3 = None
    cam4 = None
    stream4 = None

    # Configuration
    autoMode = True


class TimeBuffer:

    def __init__(self):
        self.timestamps = np.array([])
        self.BUFFER_SIZE = 20
        self.currTime = None

    def push(self, timestamp):
        if self.currTime is None:
            pass
        elif len(self.timestamps) < self.BUFFER_SIZE:
            self.timestamps = np.append(
                self.timestamps, timestamp - self.currTime)
        else:
            self.timestamps[:-1] = self.timestamps[1:]
            self.timestamps[-1] = timestamp - self.currTime
        self.currTime = timestamp

    def get_fps(self):
        if len(self.timestamps) == 0:
            return 0
        return 1/np.average(self.timestamps)


class RTTCalculator:

    def __init__(self, webSocket, interval):
        self.webSocket = webSocket
        self.interval = interval
        self.lastSent = None
        self._isRunning = False
        self.count = 0

    def sendPing(self):
        self.lastSent = time.time()
        self.webSocket.write_message("ping")


class CamSocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):

        self.camName = "INVALID"
        if (self.request.path == "/cam0"):
            self.camName = "cam0"
            StateManager.cam0 = self
        elif (self.request.path == "/cam1"):
            self.camName = "cam1"
            StateManager.cam1 = self
        elif (self.request.path == "/cam2"):
            self.camName = "cam2"
            StateManager.cam2 = self
        elif (self.request.path == "/cam3"):
            self.camName = "cam3"
            StateManager.cam3 = self
        elif (self.request.path == "/cam4"):
            self.camName = "cam4"
            StateManager.cam4 = self

        LOG(self.camName + " Opened")

        # Initialize time buffer to track FPS
        self.timeBuffer = TimeBuffer()

        # Initialize RTT Calculator to track RTT
        self.rttCalculator = RTTCalculator(self, 1)

        # Configure and start periodic RTT ping
        self.periodicPing = tornado.ioloop.PeriodicCallback(
            self.rttCalculator.sendPing, 1000)
        self.periodicPing.start()

        # Iniaitlize current RTT to 0
        self.rtt = 0

    def on_close(self):

        LOG(self.camName + " Closed")

        # Stop sending RTT ping
        self.periodicPing.stop()

        if (self.request.path == "/cam0"):
            StateManager.cam0 = None
        elif (self.request.path == "/cam1"):
            StateManager.cam1 = None
        elif (self.request.path == "/cam2"):
            StateManager.cam2 = None
        elif (self.request.path == "/cam3"):
            StateManager.cam3 = None
        elif (self.request.path == "/cam4"):
            StateManager.cam4 = None

    def on_message(self, message):

        stream = None
        if (self.request.path == "/cam0" and StateManager.stream0):
            stream = StateManager.stream0
        elif (self.request.path == "/cam1" and StateManager.stream1):
            stream = StateManager.stream1
        elif (self.request.path == "/cam2" and StateManager.stream2):
            stream = StateManager.stream2
        elif (self.request.path == "/cam3" and StateManager.stream3):
            stream = StateManager.stream3
        elif (self.request.path == "/cam4" and StateManager.stream4):
            stream = StateManager.stream4

        if stream:

            # If message received is a ping
            if (message == "ping"):
                # Update RTT value
                self.rtt = time.time() - self.rttCalculator.lastSent
                # DEBUG(self.rtt)

            # If message received is an image
            else:
                # Decode received image
                data = np.asarray(bytearray(message), dtype="uint8")
                image = cv2.imdecode(data, cv2.IMREAD_COLOR)

                # If images are from Cam0, do:
                if (self.request.path == "/cam0"):
                    pass
                # Else, do:
                else:
                    pass

                # Calculate FPS
                self.timeBuffer.push(time.time())
                fps = self.timeBuffer.get_fps()

                # Put FPS and RTT onto image
                fpsPos = (10, 20)
                rttPos = (10, 40)
                fontFace = cv2.FONT_HERSHEY_SIMPLEX
                fontScale = 0.5
                color = (255, 0, 0)
                thickness = 1
                lineType = cv2.LINE_AA
                image = cv2.putText(image, "FPS: {:.1f}".format(fps), fpsPos, fontFace,
                                    fontScale, color, thickness, lineType)
                image = cv2.putText(image, "RTT: {:.0f}".format(self.rtt * 1000), rttPos, fontFace,
                                    fontScale, color, thickness, lineType)

                # Convert to JPEG
                encoded_image = cv2.imencode('.jpg', image)[1]
                bytes_image = np.array(encoded_image).tobytes()

                # Convert to base64 encoding and send image to webpage
                payload = base64.b64encode(bytes_image)
                stream.write_message(payload)
                # print("sent")


class SentrySocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):
        LOG("Sentry Opened")
        StateManager.sentry = self
        self.timeBuffer = TimeBuffer()

    def on_close(self):
        LOG("Sentry Closed")
        StateManager.sentry = None

    def on_message(self, message):
        DEBUG("Received:", message)


class StreamSocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):

        self.streamName = "INVALID"
        if (self.request.path == "/stream0"):
            self.streamName = "stream0"
            StateManager.stream0 = self
        elif (self.request.path == "/stream1"):
            self.streamName = "stream1"
            StateManager.stream1 = self
        elif (self.request.path == "/stream2"):
            self.streamName = "stream2"
            StateManager.stream2 = self
        elif (self.request.path == "/stream3"):
            self.streamName = "stream3"
            StateManager.stream3 = self
        elif (self.request.path == "/stream4"):
            self.streamName = "stream4"
            StateManager.stream4 = self

        LOG(self.streamName + " Opened")

    def on_close(self):

        LOG(self.streamName + " Closed")

        if (self.request.path == "/stream0"):
            StateManager.stream0 = None
        elif (self.request.path == "/stream1"):
            StateManager.stream1 = None
        elif (self.request.path == "/stream2"):
            StateManager.stream2 = None
        elif (self.request.path == "/stream3"):
            StateManager.stream3 = None
        elif (self.request.path == "/stream4"):
            StateManager.stream4 = None

    def on_message(self, message):

        if (self.request.path != "/stream0"):
            return

        if message == "manualOn":
            StateManager.autoMode = False
        if message == "left":
            DEBUG("left")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("left")
        if message == "right":
            DEBUG("right")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("right")
        if message == "up":
            DEBUG("up")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("up")
        if message == "down":
            DEBUG("down")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("down")
        if message == "fire":
            DEBUG("fire")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("fire")


if __name__ == "__main__":
    settings = {
        "template_path": os.path.join(os.path.dirname(__file__), "templates"),
        "static_path": os.path.join(os.path.dirname(__file__), "static"),
    }
    application = tornado.web.Application(
        handlers=[(r"/", MainHandler),
                  (r"/sentry", SentrySocketHandler),
                  (r"/cam0", CamSocketHandler),
                  (r"/stream0", StreamSocketHandler),
                  (r"/cam1", CamSocketHandler),
                  (r"/stream1", StreamSocketHandler),
                  (r"/cam2", CamSocketHandler),
                  (r"/stream2", StreamSocketHandler),
                  (r"/cam3", CamSocketHandler),
                  (r"/stream3", StreamSocketHandler),
                  (r"/cam4", CamSocketHandler),
                  (r"/stream4", StreamSocketHandler),
                  ],
        **settings
    )
    application.listen(8888)
    tornado.ioloop.IOLoop.current().start()
