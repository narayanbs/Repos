####
# yield from acts as if the calling code
# yields from reader directly instead of the wrapper


def reader():
    for i in range(4):
        yield "<< {}".format(i)


def reader_wrapper(g):
    # for v in g:
    #     yield v
    yield from g


wrap = reader_wrapper(reader())
for i in wrap:
    print(i)
