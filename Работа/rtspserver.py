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


pipeline_str = "( v4l2src device=/dev/video0 ! video/x-raw, width=640, height=480, framerate=10/1 ! \
                                rsvgoverlay name=overlay ! v4l2h264enc ! rtph264pay name=pay0 pt=96 )"

pipeline_str2 = "( v4l2src device=/dev/video0 ! video/x-raw, width=640, height=480, framerate=10/1, pixel-aspect-ratio=1/1 ! \
                                gdkpixbufoverlay location=battery.png offset-x=0 offset-y=0 overlay-height=40 overlay-width=40 ! v4l2h264enc ! rtph264pay name=pay0 pt=96 )"


class PotatoCamFactory(GstRtspServer.RTSPMediaFactory):
    def __init__(self):
        print("PotatoCamFactory_init")
        GstRtspServer.RTSPMediaFactory.__init__(self)
        self.overlay = None
        self.isStart = False


    def do_create_element(self, url):
        pipeline = Gst.parse_launch(pipeline_str)
        print(pipeline_str)
        self.overlay = pipeline.get_by_name('overlay')
        self.isStart = True
        self.thread = MyThread(self.overlay)
        self.thread.start()
        print("PotatoCamFactory_do_create_element")
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



# Порт: 5554. Камера: potato.
class PotatoServer():
    def __init__(self):
        print("PotatoServer")
        self.PotatoServer = GstRtspServer.RTSPServer.new()
        self.PotatoServer.set_service('5554')

        self.potatoCam = PotatoCamFactory()
        self.potatoCam.set_shared(True)

        m = self.PotatoServer.get_mount_points()
        m.add_factory("/potato", self.potatoCam)

        self.PotatoServer.attach(None)

        port_PotatoServer = self.PotatoServer.get_bound_port()
        print("PotatoServer готов. Порт сервера:", port_PotatoServer, "/potato")


def loopStart(start, overlay):
    temp = False
    while temp == False:
        if start == True:
            print("MyThread_Run")
            p = 0
            while p < 901:
                svg_canvas = svgwrite.Drawing('test.svg', size=(W, H))
                svg_canvas.add(svg_canvas.text(p, insert=(50, 50), font_size='40px', fill='red', font_family=FONT))
                overlay.set_property('data', svg_canvas.tostring())
                p = p + 1
                time.sleep(1)



if __name__ == '__main__':

    print(4)
    s1 = PotatoServer()

    loop.run()
    print(6)
