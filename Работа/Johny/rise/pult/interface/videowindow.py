import gi
import sys
import numpy
import time

gi.require_version("Gst", "1.0")
gi.require_version('GstVideo', '1.0')
gi.require_version('Gtk', '3.0')
from gi.repository import GdkX11, GstVideo  # не удалять эту строку, иначе все полетит
from gi.repository import Gst, GObject, Gtk, Gdk


def fullscreenAtMonitor(window, n):
    screen = Gdk.Screen.get_default()
    if screen.get_n_monitors() < 2:
        raise ValueError("HDMI экран не найден")
    monitorGeo = screen.get_monitor_geometry(n)
    x = monitorGeo.x
    y = monitorGeo.y
    window.move(x, y)
    window.fullscreen()


class VideoWindow:
    def __init__(self, ip, rtpports=(5000, 5001, 5005), codec="JPEG"):
        self._ip = ip
        self._ports = rtpports
        self._codec = codec  # используемый кодек

        self._window = Gtk.Window(Gtk.WindowType.TOPLEVEL)
        self._window.set_title("VR-player")
        self._videoWindow = Gtk.DrawingArea()
        self._window.add(self._videoWindow)

        fullscreenAtMonitor(self._window, 1)  # на втором мониторе
        self._window.show_all()

        Gst.init(sys.argv)  # Инициализация компонентов
        GObject.threads_init()

        if self._codec == "JPEG":
            self._videoCaps = 'application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)JPEG,' \
                             'payload=(int)26'
        elif self._codec == "H264":
            pass
        else:
            raise ValueError(self._codec.__repr__(), ": такого кодека нет")

        self._player = None

    def _buildPipe(self):
        self._player = Gst.Pipeline.new("player")  # создаем pipeline

        videodepay0 = None

        if self._codec == "JPEG":
            videodepay0 = Gst.ElementFactory.make('rtpjpegdepay', 'videodepay0')  # создаем раскпаковщик видео формата jpeg
        elif self._codec == "H264":
            pass
        else:
            raise ValueError(self._codec.__repr__(), ": такого кодека нет")

        rtpbin = Gst.ElementFactory.make('rtpbin', 'rtpbin')  # создаем rtpbin
        self._player.add(rtpbin)  # добавляем его в Pipeline
        caps = Gst.caps_from_string(self._videoCaps)  # в каком формате принимать видео

        def pad_added_cb(rtpbin, new_pad, depay):
            sinkpad = Gst.Element.get_static_pad(depay, 'sink')
            Gst.Pad.link(new_pad, sinkpad)

        rtpsrc0 = Gst.ElementFactory.make('udpsrc', 'rtpsrc0')
        rtpsrc0.set_property('port', self._ports[0])

        """ we need to set caps on the udpsrc for the RTP data """
        rtpsrc0.set_property('caps', caps)

        rtcpsrc0 = Gst.ElementFactory.make('udpsrc', 'rtcpsrc0')
        rtcpsrc0.set_property('port', self._ports[1])

        rtcpsink0 = Gst.ElementFactory.make('udpsink', 'rtcpsink0')
        rtcpsink0.set_property('port', self._ports[2])
        rtcpsink0.set_property('host', self._ip)

        """ no need for synchronisation or preroll on the RTCP sink """
        rtcpsink0.set_property('async', False)
        rtcpsink0.set_property('sync', False)
        self._player.add(rtpsrc0)
        self._player.add(rtcpsrc0)
        self._player.add(rtcpsink0)

        srcpad0 = Gst.Element.get_static_pad(rtpsrc0, 'src')

        sinkpad0 = Gst.Element.get_request_pad(rtpbin, 'recv_rtp_sink_0')
        Gst.Pad.link(srcpad0, sinkpad0)

        """ get an RTCP sinkpad in session 0 """
        srcpad0 = Gst.Element.get_static_pad(rtcpsrc0, 'src')
        sinkpad0 = Gst.Element.get_request_pad(rtpbin, 'recv_rtcp_sink_0')
        Gst.Pad.link(srcpad0, sinkpad0)

        """ get an RTCP srcpad for sending RTCP back to the sender """
        srcpad0 = Gst.Element.get_request_pad(rtpbin, 'send_rtcp_src_0')
        sinkpad0 = Gst.Element.get_static_pad(rtcpsink0, 'sink')
        Gst.Pad.link(srcpad0, sinkpad0)

        #rtpbin.set_property('drop-on-latency', True)
        #rtpbin.set_property('buffer-mode', 1)

        rtpbin.connect('pad-added', pad_added_cb, videodepay0)

        decoder0 = None

        if self._codec == "JPEG":
            decoder0 = Gst.ElementFactory.make('jpegdec', "decoder0")
        elif self._codec == "H264":
            pass
        else:
            raise ValueError(self._codec.__repr__(), ": такого кодека нет")

        videoconvert0 = Gst.ElementFactory.make("videoconvert", "videoconvert0")
        videoscale0 = Gst.ElementFactory.make("videoscale", "videoscale0")  # растягиваем изображение
        sink = Gst.ElementFactory.make("autovideosink", "sink")

        self._player.add(videodepay0)  # добавляем все элементы в pipeline
        self._player.add(decoder0)
        self._player.add(videoscale0)
        self._player.add(videoconvert0)
        self._player.add(sink)

        self.bus = self._player.get_bus()  # создаем шину передачи сообщений и ошибок от GST
        self.bus.add_signal_watch()
        self.bus.enable_sync_message_emission()
        self.bus.connect("message::error", self.__on_error)
        self.bus.connect("message::eos", self.__on_eos)
        self.bus.connect("sync-message::element", self.__on_sync_message)

        videodepay0.link(decoder0)
        decoder0.link(videoconvert0)
        videoconvert0.link(videoscale0)
        videoscale0.link(sink)

    def start(self):
        if not self._player:
            self._buildPipe()
            self._player.set_state(Gst.State.PLAYING)
        else:
            state = self._player.get_state(Gst.CLOCK_TIME_NONE).state  # текущее состояние pipeline
            if state == Gst.State.PAUSED:  # если перед этим была вызвана пауза
                self._player.set_state(Gst.State.PLAYING)
            elif state == Gst.State.PLAYING:  # если видос уже запущен
                raise BrokenPipeError("Нельзя повторно запустить видео")
            else:  # если перед этим было вызвано stop
                self._buildPipe()  # запустить pipeline
                self._player.set_state(Gst.State.PLAYING)

    def paused(self):
        if self._player:
            if ((
                    self._player.get_state(
                        Gst.CLOCK_TIME_NONE).state) == Gst.State.NULL):  # если перед этим было вызвано stop
                raise BrokenPipeError("Нельзя поставить на паузу освобожденные ресурсы")
            else:
                self._player.set_state(Gst.State.PAUSED)

    def stop(self):
        if self._player:
            self._player.set_state(Gst.State.NULL)
            del self._player
            self._player = None

        self._window.destroy()
        time.sleep(1)

    def __on_error(self, bus, msg):  # прием ошибок
        err, dbg = msg.parse_error()
        print("ERROR:", msg.src.get_name(), ":", err.message)  # нужно ли тут вообще исключение?
        if dbg:
            print("Debug info:", dbg)

    def __on_eos(self, bus, msg):  # ловим конец передачи видео
        print("End-Of-Stream reached")
        self._player.set_state(Gst.State.READY)

    def __on_sync_message(self, bus, message):
        if message.get_structure().get_name() == 'prepare-window-handle':
            imagesink = message.src
            imagesink.set_property("force-aspect-ratio", True)
            imagesink.set_window_handle(self._videoWindow.get_property('window').get_xid())


if __name__ == "__main__":
    vw = VideoWindow("127.0.0.1")
    vw.start()
    Gtk.main()
