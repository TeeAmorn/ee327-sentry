# class Test:
#     foo = "hello, world"


# a = Test()
# b = Test()

# print(Test.foo)
# print(a.foo)
# print(b.foo, "\n")

# a.foo = 1

# print(Test.foo)
# print(a.foo)
# print(b.foo, "\n")

# del a.foo

# print(Test.foo)
# print(a.foo)
# print(b.foo, "\n")

# Test.foo = 2

# print(Test.foo)
# print(a.foo)
# print(b.foo, "\n")


from sre_parse import State


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
    def get_webpage(cls):
        return StateManager.webpage

    @classmethod
    def set_webpage(cls, webpage):
        cls.webpage = webpage


print(StateManager.get_sentry())
StateManager.set_sentry(2)
print(StateManager.get_sentry())
