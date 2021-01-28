#!/bin/sh

#DEST=127.0.0.1
#DEST=192.168.42.106
DEST=192.168.42.100

VELEM_L="v4l2src do-timestamp=false device=/dev/video0"
VELEM_R="v4l2src do-timestamp=false device=/dev/video1"
#VCAPS="image/jpeg, width=(int)1280, height=(int)720, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)10/1"
VCAPS="image/jpeg, width=(int)960, height=(int)544, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)10/1"
VENC="rtpjpegpay"

VRTPSINK_L="udpsink port=5000 host=$DEST"
VRTCPSINK_L="udpsink port=5001 host=$DEST sync=false async=false"
VRTCPSRC_L="udpsrc port=5005"

VRTPSINK_R="udpsink port=6000 host=$DEST"
VRTCPSINK_R="udpsink port=6001 host=$DEST sync=false async=false"
VRTCPSRC_R="udpsrc port=6005"

#RTPBIN_PARAMS="ntp-sync=true"
RTPBIN_PARAMS="ntp-sync=true do-retransmition=true ntp-time-source=3 rtcp-sync-sent-time=false"
#RTPBIN_PARAMS="ntp-sync=true do-retransmition=true"

gst-launch-1.0 -v \
rtpbin name=rtpbinleft $RTPBIN_PARAMS \
    $VELEM_L ! $VCAPS ! $VENC ! rtpbinleft.send_rtp_sink_0 \
        rtpbinleft.send_rtp_src_0 ! $VRTPSINK_L \
        rtpbinleft.send_rtcp_src_0 ! $VRTCPSINK_L \
    $VRTCPSRC_L ! rtpbinleft.recv_rtcp_sink_0 \
rtpbin name=rtpbinright $RTPBIN_PARAMS \
    $VELEM_R ! $VCAPS ! $VENC ! rtpbinright.send_rtp_sink_1 \
        rtpbinright.send_rtp_src_1 ! $VRTPSINK_R \
        rtpbinright.send_rtcp_src_1 ! $VRTCPSINK_R \
    $VRTCPSRC_R ! rtpbinright.recv_rtcp_sink_1

