import base64
import os
from sre_parse import State
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

camera_direction = ["NORTH", "EAST", "SOUTH", "WEST"]


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
    stream5 = None

    # Configuration
    autoMode = True

    # Current Images
    image0 = None
    image1 = None
    image2 = None
    image3 = None
    image4 = None


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


class SentryTracking:

    def __init__(self):
        self.face_cascade = cv2.CascadeClassifier(
            'haarcascade_frontalface_default.xml')
        self.tracker = cv2.TrackerKCF_create()
        self.foundTarget = False
        self.scanningMode = True
        self.trackingFailedCount = 0

    def track(self):

        # If we do not have an image yet or we are in manual mode, do nothing
        if StateManager.image0 is None or not StateManager.autoMode:
            DEBUG("Stream0 not found or in MAUAL mode")
            return

        # Make copy of image0
        frame = np.copy(StateManager.image0)

        # Use surrounding cameras to scan for people
        if self.scanningMode:

            # If none of the surrounding cameras are on right now, stop
            if (StateManager.cam1 is None and
                StateManager.cam2 is None and
                StateManager.cam3 is None and
                    StateManager.cam4 is None):
                self.scanningMode = False
                LOG("Surrounding cameras not on; exitting scanning mode")
                return

            # Grab image from the surrounding cameras
            images = [None] * 4
            images[0] = StateManager.image1
            images[1] = StateManager.image2
            images[2] = StateManager.image3
            images[3] = StateManager.image4

            # Search the images for a target; select one with the largest area
            cameraTarget = -1
            targetSize = -1
            camOn = False
            for cam_no, image in enumerate(images):
                if image is None:
                    continue
                camOn = True
                gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
                faces = self.face_cascade.detectMultiScale(gray, 1.1, 4)
                for face in faces:
                    if face[2]*face[3] > targetSize:
                        targetSize = face[2]*face[3]
                        cameraTarget = cam_no

            # If surrounding cameras haven't sent any images, quit scanning mode
            if not camOn:
                self.scanningMode = False
                LOG("Surrounding camera images not found; exitting scanning mode")
                return

            # If target found, exit scanning mode and tell sentry to turn to that direction
            if cameraTarget != -1:
                self.scanningMode = False
                if StateManager.sentry:
                    StateManager.sentry.write_message(
                        camera_direction[cameraTarget])
                    LOG("Sending " +
                        camera_direction[cameraTarget] + " to sentry")
                LOG("Found target in CAMERA " +
                    str(cameraTarget+1) + "; exitting scanning mode")
                return

            # Send image to stream5 if exists
            if StateManager.stream5:
                cv2.putText(frame, "Scanning Mode", (100, 80),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 2)

                # Convert to JPEG
                encoded_image = cv2.imencode('.jpg', frame)[1]
                bytes_image = np.array(encoded_image).tobytes()

                # Convert to base64 encoding and send image to webpage
                payload = base64.b64encode(bytes_image)
                StateManager.stream5.write_message(payload)

            # self.scanningMode = False

        # We're not in scanning mode, sentry takes control
        else:

            # If sentry has not found target, keep detecting
            if not self.foundTarget:

                # Convert image to grayscale and then do detection
                gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
                faces = self.face_cascade.detectMultiScale(gray, 1.1, 4)

                # If we did not detect any face, increment failed counts
                if len(faces) == 0:

                    # Increment count by 1
                    self.trackingFailedCount += 1

                    # Check if we have failed 5 times
                    if self.trackingFailedCount == 40:

                        # Reset count
                        self.trackingFailedCount = 0

                        # Change to scanning mode
                        self.scanningMode = True
                        self.foundTarget = False
                        LOG("Entering scanning mode")

                # We did detect a face, thus start tracking it
                else:

                    # Reset count
                    self.trackingFailedCount = 0

                    # Track face using bounding box
                    bbox = faces[0]
                    self.tracker = cv2.TrackerKCF_create()
                    self.tracker.init(frame, bbox)

                    # Update foundTarget
                    self.foundTarget = True
                    LOG("Found at least one face to track")

                # Send image to stream5 if exists
                if StateManager.stream5:
                    cv2.putText(frame, "Tracking failure detected", (100, 80),
                                cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 2)

                    # Convert to JPEG
                    encoded_image = cv2.imencode('.jpg', frame)[1]
                    bytes_image = np.array(encoded_image).tobytes()

                    # Convert to base64 encoding and send image to webpage
                    payload = base64.b64encode(bytes_image)
                    StateManager.stream5.write_message(payload)

            # If sentry already found target, track it
            else:

                self.foundTarget, bbox = self.tracker.update(frame)

                # Compute how far target is from the center of the screen
                horizontal_delta = bbox[0]+bbox[2]//2 - frame.shape[1]//2
                vertical_delta = bbox[1]+bbox[3]//2 - frame.shape[0]//2

                # If sentry is connected, send movement commands to sentry
                if StateManager.sentry:
                    message = str(horizontal_delta) + ',' + str(vertical_delta)
                    StateManager.sentry.write_message(message)
                    LOG("Sent " + message + " command to sentry")

                # If stream5 exists, draw bounding box around image and send it
                if StateManager.stream5:

                    # Draw bounding box
                    p1 = (int(bbox[0]), int(bbox[1]))
                    p2 = (int(bbox[0] + bbox[2]), int(bbox[1] + bbox[3]))
                    cv2.rectangle(frame, p1, p2, (255, 0, 0), 2, 1)

                    # Convert to JPEG
                    encoded_image = cv2.imencode('.jpg', frame)[1]
                    bytes_image = np.array(encoded_image).tobytes()

                    # Convert to base64 encoding and send image to webpage
                    payload = base64.b64encode(bytes_image)
                    StateManager.stream5.write_message(payload)


class CamSocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):

        self.camName = "INVALID"
        if (self.request.path == "/cam0"):
            self.camName = "cam0"
            StateManager.cam0 = self
            self.sentryTracking = SentryTracking()
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

                # Update current image state
                if (self.request.path == "/cam0"):
                    StateManager.image0 = image
                elif (self.request.path == "/cam1"):
                    StateManager.image1 = image
                elif (self.request.path == "/cam2"):
                    StateManager.image2 = image
                elif (self.request.path == "/cam3"):
                    StateManager.image3 = image
                elif (self.request.path == "/cam4"):
                    StateManager.image4 = image

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

        if (StateManager.autoMode and self.request.path == "/cam0" and StateManager.stream0):
            self.sentryTracking.track()


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
        elif (self.request.path == "/stream5"):
            self.streamName = "stream5"
            StateManager.stream5 = self

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
        elif (self.request.path == "/stream5"):
            StateManager.stream5 = None

    def on_message(self, message):

        if (self.request.path != "/stream0"):
            return

        if message == "manualOn":
            LOG("Turning MANUAL mode ON")
            StateManager.autoMode = False
        if message == "manualOff":
            LOG("Turning MANUAL mode OFF")
            StateManager.autoMode = True
        if message == "left":
            DEBUG("Sending MANUAL command LEFT to the sentry")
            StateManager.autoMode = False
            if StateManager.sentry:
                StateManager.sentry.write_message("-60,0")
        if message == "right":
            DEBUG("Sending MANUAL command RIGHT to the sentry")
            StateManager.autoMode = False
            if StateManager.sentry:
                StateManager.sentry.write_message("60,0")
        if message == "up":
            DEBUG("Sending MANUAL command UP to the sentry")
            StateManager.autoMode = False
            if StateManager.sentry:
                StateManager.sentry.write_message("0,-60")
        if message == "down":
            DEBUG("Sending MANUAL command DOWN to the sentry")
            StateManager.autoMode = False
            if StateManager.sentry:
                StateManager.sentry.write_message("0,60")
        if message == "fire":
            DEBUG("Sending MANUAL command FIRE to the sentry")
            StateManager.autoMode = False
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
                  (r"/stream5", StreamSocketHandler)
                  ],
        **settings
    )
    application.listen(8888)
    tornado.ioloop.IOLoop.current().start()
