#!/usr/bin/env python
# -*- coding:utf-8 vi:ts=4:noexpandtab
# Simple RTSP server. Run as-is or with a command-line to replace the default pipeline

import sys
import gi
import threading
import time
import svgwrite
gi.require_version('Gst', '1.0')
gi.require_version('GstRtspServer', '1.0')
from gi.repository import Gst, GstRtspServer, GLib
from PIL import ImageFont
from multiprocessing import Process

loop = GLib.MainLoop()
# GLib.threads_init()
Gst.init(None)

W = 640
H = 480
COLOR = 'lime'
FONT = 'arial'
FSIZE = '30px'
TIMER_SEC = 10
font = ImageFont.truetype("arial.ttf", 30)


piplineRtspEduBot = "v4l2src device=/dev/video0 ! video/x-raw, width=320, height=240, framerate=10/1, pixel-aspect-ratio=1/1 ! \
                                gdkpixbufoverlay location=battery.png offset-x=0 offset-y=0 overlay-height=40 overlay-width=40 ! v4l2h264enc ! rtph264pay name=pay0 pt=96"


class PotatoCamFactory(GstRtspServer.RTSPMediaFactory):
    def __init__(self):
        GstRtspServer.RTSPMediaFactory.__init__(self)

    def do_create_element(self, url):
        pipeline_str = "v4l2src device=/dev/video0 ! video/x-raw, width=320, height=240, framerate=10/1, pixel-aspect-ratio=1/1 !\
                                 ! videoconvert ! rsvgoverlay name=overlay ! videoconvert ! v4l2h264enc ! rtph264pay name=pay0 pt=96"
        pipeline = Gst.parse_launch(pipeline_str)
        overlay = pipeline.get_by_name('overlay')
        self.thread = MyThread(overlay)
        self.thread.start()
        print(pipeline_str)
        return pipeline



class MyThread(threading.Thread):
    def __init__(self, overlay):
        threading.Thread.__init__(self)
        self.overlay = overlay
        print("MyThread_Init")

    def run(self):
        print("MyThread_Run")
        p = 0
        while p < 901:
            # overlay = self.pipeline.get_by_name('overlay')
            svg_canvas = svgwrite.Drawing('test.svg', size=(W, H))
            svg_canvas.add(svg_canvas.text(p, insert=(50, 50), font_size='40px', fill='red', font_family=FONT))
            self.overlay.set_property('data', svg_canvas.tostring())
            p = p + 1
            time.sleep(1)

"""
class test(threading.Thread):
    def __init__(self, overlay):
        threading.Thread.__init__(self)
        self.overlay = overlay
        print("MyThread_Init")

    def run(self):
        print("MyThread_Run")
        p = 0
        while p < self.overlay:
            p = p + 1
            print(p)
            time.sleep(1)
"""

# Порт: 5554. Камера: potato.
class PotatoServer():
    def __init__(self):
        self.PotatoServer = GstRtspServer.RTSPServer.new()
        self.PotatoServer.set_service('5554')

        potatoCam = PotatoCamFactory()
        potatoCam.set_shared(True)

        m = self.PotatoServer.get_mount_points()
        m.add_factory("/potato", potatoCam)

        self.PotatoServer.attach(None)

        port_PotatoServer = self.PotatoServer.get_bound_port()
        print("PotatoServer готов. Порт сервера:", port_PotatoServer, "/potato")


if __name__ == '__main__':

    print(4)
    s1 = PotatoServer()

    loop.run()
    print(6)