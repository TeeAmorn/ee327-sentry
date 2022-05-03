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
SET_DEBUG = False


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
    sentry = None
    webpage = None
    cam1 = None
    cam2 = None
    cam3 = None
    cam4 = None
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


class StreamSocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):
        LOG("Cam Opened")
        StateManager.cam1 = self
        self.timeBuffer = TimeBuffer()
        self.rttCalculator = RTTCalculator(self, 1)
        self.periodicPing = tornado.ioloop.PeriodicCallback(
            self.rttCalculator.sendPing, 1000)
        self.periodicPing.start()
        self.rtt = 0

    def on_close(self):
        LOG("Cam Closed")
        self.periodicPing.stop()
        StateManager.cam1 = None

    def on_message(self, message):
        if StateManager.webpage:

            # If message received is a ping
            if (message == "ping"):
                # Update RTT value
                self.rtt = time.time() - self.rttCalculator.lastSent
                DEBUG(self.rtt)

            # If message received is an image
            else:
                # Decode received image
                data = np.asarray(bytearray(message), dtype="uint8")
                image = cv2.imdecode(data, cv2.IMREAD_COLOR)

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
                StateManager.webpage.write_message(payload)


class CamSocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):
        LOG("Cam 1 Opened")
        StateManager.cam1 = self
        self.timeBuffer = TimeBuffer()
        self.rttCalculator = RTTCalculator(self, 1)
        self.periodicPing = tornado.ioloop.PeriodicCallback(
            self.rttCalculator.sendPing, 1000)
        self.periodicPing.start()
        self.rtt = 0

    def on_close(self):
        LOG("Cam Closed")
        self.periodicPing.stop()
        StateManager.cam1 = None

    def on_message(self, message):
        if StateManager.webpage:

            # If message received is a ping
            if (message == "ping"):
                # Update RTT value
                self.rtt = time.time() - self.rttCalculator.lastSent
                DEBUG(self.rtt)

            # If message received is an image
            else:
                # Decode received image
                data = np.asarray(bytearray(message), dtype="uint8")
                image = cv2.imdecode(data, cv2.IMREAD_COLOR)

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
                StateManager.webpage.write_message(payload)


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


class WebpageSocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):
        LOG("Webpage Opened")
        StateManager.webpage = self

    def on_close(self):
        LOG("Webpage Closed")
        StateManager.webpage = None

    def on_message(self, message):
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
                  (r"/webpage", WebpageSocketHandler),
                  (r"/stream", StreamSocketHandler)],
        **settings
    )
    application.listen(8888)
    tornado.ioloop.IOLoop.current().start()
