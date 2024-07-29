import textwrap


def get_string():
    return textwrap.dedent(
        """\
        This
        is a multiline

        string"""
    )


print(get_string())
