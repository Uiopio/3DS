#!/bin/sh

#DEST=127.0.0.1
DEST=192.168.42.100
#DEST=192.168.42.108

VELEM_L="v4l2src device=/dev/video0"

VCAPS="image/jpeg, width=(int)1280, height=(int)480, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1"
#VCAPS="video/x-raw, width=(int)1280, height=(int)480, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1, encoding-name=(string)JPEG"
#VCAPS="image/jpeg, width=(int)640, height=(int)480, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1"
#VCAPS="image/jpeg, width=(int)320, height=(int)240, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1"
VENC="jpegdec ! jpegenc ! rtpjpegpay"

VRTPSINK_L="udpsink port=5000 host=$DEST name=vrtpsink_l"
VRTCPSINK_L="udpsink port=5001 host=$DEST sync=false async=false name=vrtcpsink_l"
VRTCPSRC_L="udpsrc port=5005 name=vrtcpsrc_l"

#RTPBIN_PARAMS="ntp-sync=true"
#RTPBIN_PARAMS="ntp-sync=true ntp-time-source=3 rtcp-sync-sent-time=false do-retransmission=false"
#RTPBIN_PARAMS="ntp-sync=true do-retransmition=true"
RTPBIN_PARAMS=""
#RTPBIN_PARAMS="buffer-mode=1"



gst-launch-1.0 -v \
rtpbin name=rtpbin $RTPBIN_PARAMS \
    $VELEM_L ! $VCAPS ! $VENC ! rtpbin.send_rtp_sink_0 \
        rtpbin.send_rtp_src_0 ! $VRTPSINK_L \
        rtpbin.send_rtcp_src_0 ! $VRTCPSINK_L \
    $VRTCPSRC_L ! rtpbin.recv_rtcp_sink_0 
    
