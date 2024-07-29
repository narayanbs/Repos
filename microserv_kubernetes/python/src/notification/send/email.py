import os
import smtplib
import json

# Import the email modules we'll need
from email.message import EmailMessage


def notification(message):
    try:
        message = json.loads(message)
        mp3_fid = message["mp3_fid"]
        sender_address = os.environ.get("GMAIL_ADDRESS")
        sender_password = os.environ.get("GMAIL_PASSWORD")
        receiver_address = message["username"]

        msg = EmailMessage()
        msg.set_content("mp3 file_id: {} is now ready".format(mp3_fid))

        msg["Subject"] = "MP3_Download"
        msg["From"] = sender_address
        msg["To"] = receiver_address

        s = smtplib.SMTP("smtp.gmail.com", 587)
        s.starttls()
        s.login(sender_address, sender_password)
        s.send_message(msg, sender_address, receiver_address)
        s.quit()
        print("Mail sent")

    except Exception as err:
        print(err)
        return err
