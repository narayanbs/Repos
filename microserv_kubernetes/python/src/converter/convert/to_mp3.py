import json
import os
import tempfile

import moviepy.editor
import pika
from bson.objectid import ObjectId


def start(message, fs_videos, fs_mp3s, channel):
    message = json.loads(message)

    # empty temp file
    tf = tempfile.NamedTemporaryFile()

    out = fs_videos.get(ObjectId(message["video_fid"]))
    tf.write(out.read())

    audio = moviepy.editor.VideoFileClip(tf.name).audio

    tf.close()

    # write audo to file
    tf_path = tempfile.gettempdir() + "/{}.mp3".format(message["video_fid"])
    if audio:
        audio.write_audiofile(tf_path)
    else:
        return "failed to write audio file"

    # save to mongo
    f = open(tf_path, "rb")
    data = f.read()
    fid = fs_mp3s.put(data)
    f.close()
    os.remove(tf_path)

    message["mp3_fid"] = str(fid)

    try:
        channel.basic_publish(
            exchange="",
            routing_key=os.environ.get("MP3_QUEUE"),
            body=json.dumps(message),
            proprties=pika.BasicProperties(delivery_mode=pika.DeliveryMode.Persistent),
        )
    except Exception as err:
        print(err)
        fs_mp3s.delete(fid)
        return "failed to publish message"
