import re
from functools import partial


def some_func(gender, name, age):
    print(gender, name, age)


s = "/people/(?P<name>[^/]+)/(?P<age>[^/]+)"

route_re = re.compile(f"^{s}$")
m = route_re.match("/people/Jim/32")
if m is not None:
    params = m.groupdict()
    p = partial(some_func, **params)
    p("maley")
