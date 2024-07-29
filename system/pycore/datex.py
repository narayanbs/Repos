from datetime import datetime
from zoneinfo import ZoneInfo

from dateutil.relativedelta import relativedelta

PYCON_DATE = datetime.strptime("May 12, 2024 8:00 AM", "%B %d, %Y %H:%M %p")
PYCON_DATE = PYCON_DATE.astimezone(ZoneInfo("America/New_York"))


def time_amount(time_unit, countdown):
    t = getattr(countdown, time_unit)
    return f"{t} {time_unit}" if t != 0 else ""


def main():
    now = datetime.now().astimezone()
    countdown = relativedelta(PYCON_DATE, now)
    time_units = ["years", "months", "days", "hours", "minutes", "seconds"]
    output = (t for tu in time_units if (t := time_amount(tu, countdown)))
    pycon_date_str = PYCON_DATE.strftime("%A, %B %d, %Y at %H:%M %p %Z")
    print(f"Pycon us 2024 will start on:", pycon_date_str)
    print("Countdown to Pycon us 2024:", " ".join(output))


if __name__ == "__main__":
    main()
