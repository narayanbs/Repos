def inner():
    yield 1
    yield 2


def outer_yield():
    yield "start"
    for value in inner():
        yield value
    yield "end"


print(list(outer_yield()))


def outer_yield_same_as_above():
    yield "start"
    yield from inner()
    yield "end"


print(list(outer_yield_same_as_above()))

print("-------------------------------")


def inner_advanced():
    yield 1
    yield 2
    return "Inner Done"


def outer_advanced():
    result = yield from inner_advanced()
    print("Inner returned -->", result)
    yield "Outer Done"


gen = outer_advanced()
for val in gen:
    print("Yielded", val)
