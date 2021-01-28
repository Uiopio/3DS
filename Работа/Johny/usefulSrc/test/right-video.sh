#!/bin/sh

#DEST=127.0.0.1
DEST=192.168.42.106

VOFFSET=0
AOFFSET=0

#VELEM="v4l2src do-timestamp=true"
VELEM="v4l2src device=/dev/video1" 
#VCAPS="video/x-h264,width=1280,height=720,framerate=30/1"
VCAPS="image/jpeg, width=(int)1280, height=(int)720, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1"
VSOURCE="$VELEM ! $VCAPS"
#VENC="h264parse ! rtph264pay"
VENC="rtpjpegpay"

VRTPSINK="udpsink port=5000 host=$DEST ts-offset=$VOFFSET name=vrtpsink"
VRTCPSINK="udpsink port=5001 host=$DEST sync=false async=false name=vrtcpsink"
VRTCPSRC="udpsrc port=5005 name=vrtpsrc"

RTPBIN_PARAMS="do-retransmition=true"

gst-launch-1.0 -v rtpbin name=rtpbin $RTPBIN_PARAMS\
    $VSOURCE ! $VENC ! queue ! rtpbin.send_rtp_sink_0 \
        rtpbin.send_rtp_src_0 ! $VRTPSINK \
        rtpbin.send_rtcp_src_0 ! $VRTCPSINK \
    $VRTCPSRC ! rtpbin.recv_rtcp_sink_0
