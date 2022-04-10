import tornado.ioloop
import tornado.web
import tornado.websocket
import numpy as np
import cv2 as cv


class MainHandler(tornado.web.RequestHandler):

    def check_origin(self, origin): return True

    def get(self):
        self.render("index.html")


class StateManager:
    sentry = None
    webpage = None

    @classmethod
    def get_sentry(cls):
        return cls.sentry

    @classmethod
    def set_sentry(cls, sentry):
        cls.sentry = sentry

    @classmethod
    def clear_sentry(cls):
        cls.sentry = None

    @classmethod
    def get_webpage(cls):
        return StateManager.webpage

    @classmethod
    def set_webpage(cls, webpage):
        cls.webpage = webpage

    @classmethod
    def clear_webpage(cls):
        cls.webpage = None

    @classmethod
    def is_ready(cls):
        return cls.sentry is not None and cls.webpage is not None


class SentrySocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):
        print("Sentry Opened")
        StateManager.set_sentry(self)

    def on_close(self):
        print("Sentry Closed")
        StateManager.clear_sentry()

    def on_message(self, message):
        print(message)


class WebpageSocketHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin): return True

    def open(self):
        print("Webpage Opened")
        StateManager.set_webpage(self)

    def on_close(self):
        print("Webpage Closed")
        StateManager.clear_webpage()

    def on_message(self, message):
        pass


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
    application = tornado.web.Application([
        (r"/", MainHandler),
        (r"/sentry", SentrySocketHandler),
        (r"/webpage", WebpageSocketHandler),
        (r"/chat", ChatSocketHandler)
    ])
    application.listen(8888)
    tornado.ioloop.IOLoop.current().start()
