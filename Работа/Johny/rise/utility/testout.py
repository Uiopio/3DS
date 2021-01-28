import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject
import sys
import time
from video import Video, VIDEO_OUT_LAUNCH
Gst.init(sys.argv)

video = Video()
video.start(VIDEO_OUT_LAUNCH.format(device="/dev/video0", ip="192.168.42.100"))


while True:
    time.sleep(10)

