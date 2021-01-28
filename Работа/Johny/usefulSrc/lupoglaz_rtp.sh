#!/bin/sh

#DEST=127.0.0.1
#DEST=192.168.42.108
#DEST=192.168.42.100
#DEST=192.168.42.16
DEST=192.168.42.29

VELEM_L="v4l2src do-timestamp=true device=/dev/video0"
VELEM_R="v4l2src do-timestamp=true device=/dev/video1"
#VCAPS="image/jpeg, width=(int)1280, height=(int)720, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1"
VCAPS="image/jpeg, width=(int)640, height=(int)480, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1"
#VCAPS="image/jpeg, width=(int)320, height=(int)240, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1"
VENC="rtpjpegpay"

VRTPSINK_L="udpsink port=5000 host=$DEST name=vrtpsink_l"
VRTCPSINK_L="udpsink port=5001 host=$DEST sync=false async=false name=vrtcpsink_l"
VRTCPSRC_L="udpsrc port=5005 name=vrtcpsrc_l"

VRTPSINK_R="udpsink port=6000 host=$DEST name=vrtpsink_r"
VRTCPSINK_R="udpsink port=6001 host=$DEST sync=false async=false name=vrtcpsink_r"
VRTCPSRC_R="udpsrc port=6005 name=vrtcpsrc_r"

#RTPBIN_PARAMS="ntp-sync=true"
#RTPBIN_PARAMS="ntp-sync=true ntp-time-source=3 rtcp-sync-sent-time=false do-retransmission=false"
#RTPBIN_PARAMS="ntp-sync=true do-retransmition=true"
RTPBIN_PARAMS=""
#RTPBIN_PARAMS="drop-on-latency=true buffer-mode=0"

gst-launch-1.0  \
rtpbin name=rtpbin $RTPBIN_PARAMS \
    $VELEM_L ! $VCAPS ! $VENC ! rtpbin.send_rtp_sink_0 \
        rtpbin.send_rtp_src_0 ! $VRTPSINK_L \
        rtpbin.send_rtcp_src_0 ! $VRTCPSINK_L \
    $VRTCPSRC_L ! rtpbin.recv_rtcp_sink_0 \
    $VELEM_R ! $VCAPS ! $VENC ! rtpbin.send_rtp_sink_1 \
        rtpbin.send_rtp_src_1 ! $VRTPSINK_R \
        rtpbin.send_rtcp_src_1 ! $VRTCPSINK_R \
    $VRTCPSRC_R ! rtpbin.recv_rtcp_sink_1

