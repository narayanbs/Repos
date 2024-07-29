def writer():
    while True:
        w = yield
        print(">> ", w)


def writer_wrapper(coro):
    # coro.send(None)
    # while True:
    #     try:
    #         x = yield
    #         coro.send(x)
    #     except StopIteration:
    #         pass
    yield from coro


w = writer()
wrap = writer_wrapper(w)
wrap.send(None)  # priming the coroutine
for i in range(1, 5):
    wrap.send(i)
