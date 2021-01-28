import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject
import sys
import time

VIDEO_OUT_LAUNCH = """rtpbin name=rtpbin v4l2src device={device} !
image/jpeg, width=(int)1280, height=(int)480, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1 ! jpegdec !
jpegenc ! rtpjpegpay ! rtpbin.send_rtp_sink_0 rtpbin.send_rtp_src_0 !
udpsink port=5000 host={ip} name=vrtpsink_l rtpbin.send_rtcp_src_0 !
udpsink port=5001 host={ip} sync=false async=false name=vrtcpsink_l udpsrc port=5005 name=vrtcpsrc_l ! rtpbin.recv_rtcp_sink_0"""

VIDEO_IN_LAUNCH ="""rtspsrc location=rtsp://10.1.0.57:8554/front latency=0 buffer-mode=auto ! application/x-rtp, 
encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! autovideosink"""


class Video:
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
        #bus.add_signal_watch()
        #bus.enable_sync_message_emission()
        bus.connect("message::error", self.__on_error)
        bus.connect("message::eos", self.__on_eos)
        self._pipe.set_state(Gst.State.PLAYING)

    def stop(self):
        if not self._isConnected:
            return
        self._isConnected = False
        self._pipe.set_state(Gst.State.NULL)

    def __on_error(self, bus, msg):  # прием ошибок
        err, dbg = msg.parse_error()
        print("ERROR:", msg.src.get_name(), ":", err.message)  # нужно ли тут вообще исключение?
        if dbg:
            print("Debug info:", dbg)

    def __on_eos(self, bus, msg):  # ловим конец передачи видео
        print("End-Of-Stream reached")
        self._pipe.set_state(Gst.State.READY)
