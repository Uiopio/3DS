#!/usr/bin/env python
# -*- coding:utf-8 vi:ts=4:noexpandtab
# Simple RTSP server. Run as-is or with a command-line to replace the default pipeline

import sys
import gi

gi.require_version('Gst', '1.0')
gi.require_version('GstRtspServer', '1.0')
from gi.repository import Gst, GstRtspServer, GLib

loop = GLib.MainLoop()
#GLib.threads_init()
Gst.init(None)

class FrontCamFactory(GstRtspServer.RTSPMediaFactory):
	def __init__(self):
		GstRtspServer.RTSPMediaFactory.__init__(self)
		#self.set_shared(False)

	def do_create_element(self, url):
		s_src = "v4l2src device=/dev/video0 ! video/x-h264,width=720,height=360,framerate=15/1"
		s_h264 = "h264parse"
		a_src = "audiotestsrc ! amrnbenc ! rtpamrpay"
		pipeline_str = "( {s_src} ! queue max-size-buffers=1 name=q_enc ! {s_h264} ! rtph264pay name=pay0 pt=96 )".format(**locals())
		pipeline_str = "( v4l2src device=/dev/video0 ! video/x-h264,width=640,height=480,framerate=15/1 ! h264parse ! rtph264pay name=pay0 pt=96 audiotestsrc is-live=1 ! audio/x-raw,rate=8000 ! alawenc ! rtppcmapay name=pay1 pt=97 )"
		#pipeline_str = "( audiotestsrc is-live=1 ! audio/x-raw,rate=8000 ! alawenc ! rtppcmapay name=pay1 pt=97 )"
		pipeline_str = "( v4l2src device=/dev/video0 ! video/x-h264,width=640,height=360,framerate=10/1 ! h264parse ! rtph264pay name=pay0 pt=96 \
				alsasrc device=plughw:1,0 ! audio/x-raw, rate=16000, channels=1 ! audiochebband mode=band-pass lower-frequency=1000 upper-frequency=4000 type=2 ! audioconvert ! opusenc ! rtpopuspay name=pay1 )"
		print(pipeline_str)
		return Gst.parse_launch(pipeline_str)


class PotatoCamFactory(GstRtspServer.RTSPMediaFactory):
        def __init__(self):
                GstRtspServer.RTSPMediaFactory.__init__(self)

        def do_create_element(self, url):
                s_src = "v4l2src device=/dev/video0 ! video/x-h264,width=320,height=240,framerate=15/1"
                s_h264 = "h264parse"
                pipeline_str = "( {s_src} ! queue max-size-buffers=1 name=q_enc ! {s_h264} ! rtph264pay name=pay0 pt=96 )".format(**locals())
                print(pipeline_str)
                return Gst.parse_launch(pipeline_str)



#Порт: 8554. Камера: front.
class FrontServer():
	def __init__(self):
		self.FrontServer = GstRtspServer.RTSPServer.new()
		self.FrontServer.set_service('8554') #Выбрать порт

		frontCam = FrontCamFactory()
		frontCam.set_shared(True)


		m = self.FrontServer.get_mount_points()
		m.add_factory("/front", frontCam)

		self.FrontServer.attach(None)

		port_FrontServer = self.FrontServer.get_bound_port()
		print("FrontServer готов. Порт сервера:", port_FrontServer,"/front")


#Порт: 5554. Камера: potato.
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
		print("PotatoServer готов. Порт сервера:", port_PotatoServer,"/potato")
		
if __name__ == '__main__':
	s = FrontServer()
	s1 = PotatoServer()
	loop.run()
