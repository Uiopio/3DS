#!/bin/sh

DEST=192.168.42.108
PARAMS="rtcp-sync=0 latency=250 drop-on-latency=true buffer-mode=0"


#gst-launch-1.0 udpsrc port=7000 caps="application/x-rtp" ! queue ! rtppcmudepay ! mulawdec ! audioconvert ! alsasink

gst-launch-1.0 -v rtpbin $PARAMS name=rtpbin udpsrc caps="application/x-rtp, media=(string)audio, clock-rate=(int)8000, width=16, height=16, channels=1, channel-positions=1, payload=96" port=7000 ! rtpbin.recv_rtp_sink_2 \
rtpbin. ! queue ! rtppcmudepay ! mulawdec ! audioconvert ! autoaudiosink sync=false 
udpsrc port=7001 ! rtpbin.recv_rtp_sink_2
rtpbin.send_rtcp_src_2 ! udpsink host=$DEST port=7005 




















































