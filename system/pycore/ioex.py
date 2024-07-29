def main():
    with open("pynetw/addrex.py", "r", encoding="utf-8") as f:
        print(f.read(20))
        print("Reading another 10")
        print(f.read(10))
        f.seek(0)
        print("Reading again from the beginning")
        print(f.read(20))


if __name__ == "__main__":
    main()
