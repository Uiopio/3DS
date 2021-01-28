#!/bin/sh


DEST=192.168.42.100
PARAMS="rtcp-sync=0 latency=250 drop-on-latency=true buffer-mode=0"
VOLUME=1


gst-launch-1.0 -v rtpbin name=rtpbin                                          \
    udpsrc caps="application/x-rtp,media=(string)audio,clock-rate=(int)8000,encoding-name=(string)SPEEX"	\
														\
		port=7000 ! rtpbin.recv_rtp_sink_0								\
														\
		rtpbin. ! rtpspeexdepay ! speexdec ! audioconvert ! audioresample ! queue ! volume volume=$VOLUME  ! level ! autoaudiosink \
														\
		udpsrc port=7001 ! rtpbin.recv_rtcp_sink_0							\
		rtpbin.send_rtcp_src_0 ! udpsink port=7005 sync=false async=false



