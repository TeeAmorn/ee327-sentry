from asyncio import ThreadedChildWatcher
import base64
import os
from cv2 import FONT_HERSHEY_SCRIPT_SIMPLEX
import tornado.ioloop
import tornado.web
import tornado.websocket
import numpy as np
import cv2
import time


class MainHandler(tornado.web.RequestHandler):

    def check_origin(self, origin): return True

    def get(self):
        self.render("home.html")


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


class SentrySocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):
        print("Sentry Opened")
        StateManager.sentry = self
        self.timeBuffer = TimeBuffer()

    def on_close(self):
        print("Sentry Closed")
        StateManager.sentry = None

    def on_message(self, message):
        if StateManager.webpage:

            # Decode received image
            data = np.asarray(bytearray(message), dtype="uint8")
            image = cv2.imdecode(data, cv2.IMREAD_COLOR)

            # Calculate FPS
            self.timeBuffer.push(time.time())
            fps = self.timeBuffer.get_fps()

            # Put FPS onto image
            org = (10, 20)
            fontFace = cv2.FONT_HERSHEY_SIMPLEX
            fontScale = 0.5
            color = (255, 0, 0)
            thickness = 1
            lineType = cv2.LINE_AA
            image = cv2.putText(image, "FPS: {:.1f}".format(fps), org, fontFace,
                                fontScale, color, thickness, lineType)

            # Convert to JPEG
            encoded_image = cv2.imencode('.jpg', image)[1]
            bytes_image = np.array(encoded_image).tobytes()

            # Convert to base64 encoding and send image to webpage
            payload = base64.b64encode(bytes_image)
            StateManager.webpage.write_message(payload)


class WebpageSocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):
        print("Webpage Opened")
        StateManager.webpage = self

    def on_close(self):
        print("Webpage Closed")
        StateManager.webpage = None

    def on_message(self, message):
        if message == "manualOn":
            StateManager.autoMode = False
        if message == "left":
            print("left")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("left")
        if message == "right":
            print("right")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("right")
        if message == "up":
            print("up")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("up")
        if message == "down":
            print("down")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("down")
        if message == "fire":
            print("fire")
            StateManager.autoMode = True
            if StateManager.sentry:
                StateManager.sentry.write_message("fire")


# ==================== TEST CODE ====================

class ChatSocketHandler(tornado.websocket.WebSocketHandler):

    waiters = set()

    def check_origin(self, origin): return True

    def open(self):
        print("Chat Opened")
        ChatSocketHandler.waiters.add(self)

    def on_close(self):
        print("Chat Closed")
        ChatSocketHandler.waiters.remove(self)

    @classmethod
    def send_updates(cls, chat):
        print("Sending message to %d waiters", len(cls.waiters))
        for waiter in cls.waiters:
            try:
                waiter.write_message(chat)
            except:
                print("Error sending message", exc_info=True)

    def on_message(self, message):
        print("Got message %r", message)
        sentry = StateManager.get_sentry()
        sentry.write_message(message)

# =======================================================


if __name__ == "__main__":
    settings = {
        "template_path": os.path.join(os.path.dirname(__file__), "templates"),
        "static_path": os.path.join(os.path.dirname(__file__), "static"),
    }
    application = tornado.web.Application(
        handlers=[(r"/", MainHandler),
                  (r"/sentry", SentrySocketHandler),
                  (r"/webpage", WebpageSocketHandler),
                  (r"/chat", ChatSocketHandler)],
        **settings
    )
    application.listen(8888)
    tornado.ioloop.IOLoop.current().start()
