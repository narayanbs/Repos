import time
from datetime import date, datetime, timedelta
from zoneinfo import ZoneInfo


def main():
    print("Number of seconds since epoch", time.time())
    print("Epoch date", time.asctime(time.gmtime(0)))
    print("GMT time", time.asctime(time.gmtime()))
    print("Local time", time.asctime(time.localtime()))
    print("Inverse local time", time.mktime(time.localtime()))
    print("Current time", time.ctime(time.time()))
        #or
    print("Current time", time.ctime())

    now = datetime.now()
    print("Date time Now", now)
    print("Today's date", now.date())
    print("Todays' time", now.time())

    print(
        "Date and time in Europe/London", now.astimezone(tz=ZoneInfo("Europe/London"))
    )

    tomorrow = now + timedelta(days=+1)
    print("Tomorrow's date", tomorrow.date())
    print("Tomorrow's time", tomorrow.time())
    print("Tomorrow's date and time", tomorrow)

    somedate = datetime.strptime("01-03-2024 11:30 PM", "%d-%m-%Y %I:%M %p")
    print(type(somedate))
    print(somedate)

    print(somedate.strftime("%I:%M %p %Y-%m-%d"))


if __name__ == "__main__":
    main()
