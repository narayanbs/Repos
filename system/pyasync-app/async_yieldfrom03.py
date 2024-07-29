class SpamException(Exception):
    pass


def writer():
    while True:
        try:
            w = yield
        except SpamException:
            print("***")
        else:
            print(">> ", w)


# Not working
# def writer_wrapper(coro):
#     coro.send(None)
#     while True:
#         try:
#             x = yield
#             coro.send(x)
#         except StopIteration:
#             pass
#


# this works
# def writer_wrapper(coro):
#     coro.send(None)
#     while True:
#         try:
#             try:
#                 x = yield
#             except Exception as e:
#                 coro.throw(e)
#             else:
#                 coro.send(x)
#         except StopIteration:
#             pass


# And so does this
def writer_wrapper(coro):
    yield from coro


w = writer()
wrap = writer_wrapper(w)
wrap.send(None)
for i in [0, 1, 2, "spam", 4]:
    if i == "spam":
        wrap.throw(SpamException)
    else:
        wrap.send(i)
