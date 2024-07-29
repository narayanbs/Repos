from itertools import islice


def lucas():
    yield 2
    a = 2
    b = 1
    while True:
        yield b
        a, b = b, a + b


def search(iterable, predicate):
    for item in iterable:
        if predicate(item):
            return item
    raise ValueError("Not found")


def async_search(iterable, predicate):
    for item in iterable:
        if predicate(item):
            return item
        yield
    raise ValueError("Not Found")


if __name__ == "__main__":
    print(list(islice(lucas(), 10)))
    print(search(lucas(), lambda x: len(str(x)) >= 6))

    # async_search is a generator function .
    # search progresses when iterated with next
    # Final result "returned" in StopIteration payload
    g = async_search(lucas(), lambda x: x >= 10)
    next(g)
    next(g)
    next(g)

    # we can do other things if we want and return back to the iteration
    # state is maintained.
    print("Hello world")

    next(g)
    next(g)
    next(g)
    # StopIteration: 11
