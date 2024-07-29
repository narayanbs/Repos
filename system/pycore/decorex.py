class Application:
    def route(self, path):
        print("route called")

        def decorator(func):
            print("registering path", path)
            return func

        return decorator


def decor1(func):
    print("decor1 called")

    def inner():
        x = func()
        return x * x

    return inner


def decor(func):
    print("decor called")

    def inner():
        x = func()
        return 2 * x

    return inner


app = Application()


def num():
    print("num called")
    return 10

num = app.route("/tune")(decor1(decor(num)))

print(num())
