# .
# ..
# HOME
# fstat

import os
import os.path
from pathlib import Path
import stat

def fd_info():
    print("current dir", os.curdir)
    print("Parent dir", os.pardir)
    print("current working dir", os.getcwd())
    print("Home directory", os.environ["HOME"])
    print("Current User", os.environ["USER"])

    print("From pathlib current working dir", Path.cwd())

    print("current file", __file__)
    mypath = Path(__file__)
    st = mypath.stat()
    print(st)

    print("Is directory",stat.S_ISDIR(st.st_mode))
    print("Is Character File",stat.S_ISCHR(st.st_mode))
    print("Is Block File",stat.S_ISBLK(st.st_mode))
    print("Is Regular File",stat.S_ISREG(st.st_mode))
    print("file mode", stat.filemode(st.st_mode))


if __name__ == "__main__":
    fd_info()
