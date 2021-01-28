#Скрипт который запускает прием видео с сервера. Сервер запускается на роботе.
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject
import sys
import time

VIDEO_IN_LAUNCH ="""rtspsrc location=rtsp://10.1.0.24:8554/front latency=0 buffer-mode=auto ! application/x-rtp, 
encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! autovideosink"""

class RtspVideo:
    def __init__(self):
        Gst.init(sys.argv)
        self._pipe = None
        self._isConnected = False

    def start(self, l):
        if self._isConnected:
            return
        self._isConnected = True
        self._pipe = Gst.parse_launch(l)
        bus = self._pipe.get_bus()  # создаем шину передачи сообщений и ошибок от GST
        self._pipe.set_state(Gst.State.PLAYING)
