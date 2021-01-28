import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject
import sys
import time
from video import Video, VIDEO_IN_LAUNCH
Gst.init(sys.argv)

video = Video()
video.start(VIDEO_IN_LAUNCH.format(ip="127.0.0.1"))

time.sleep(10)
video.stop()

time.sleep(3)
video.start(VIDEO_IN_LAUNCH.format(ip="127.0.0.1"))
time.sleep(10)