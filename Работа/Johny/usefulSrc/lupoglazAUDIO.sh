#!/bin/sh

#DEST=127.0.0.1
#DEST=192.168.42.106
DEST=192.168.42.108

VELEM_L="v4l2src do-timestamp=true device=/dev/video0"
VELEM_R="v4l2src do-timestamp=true device=/dev/video1"
#VCAPS="image/jpeg, width=(int)1280, height=(int)720, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)10/1"
#VCAPS="image/jpeg, width=(int)960, height=(int)544, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)20/1"
VCAPS="image/jpeg, width=(int)320, height=(int)240, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)30/1"
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


#udpsrc caps="application/x-rtp, media=(string)audio, clock-rate=(int)8000, width=16, height=16, channels=1, #channel-positions=1, payload=96" port=7000 ! rtpbin.recv_rtp_sink_2 \
#rtpbin. ! queue ! rtppcmudepay ! mulawdec ! audioconvert ! autoaudiosink sync=false 
#udpsrc port=7001 ! rtpbin.recv_rtp_sink_2
#rtpbin.send_rtcp_src_2 ! udpsink host=$DEST port=7005 


gst-launch-1.0 -v \
rtpbin name=rtpbin $RTPBIN_PARAMS \
    $VELEM_L ! $VCAPS ! $VENC ! rtpbin.send_rtp_sink_0 \
        rtpbin.send_rtp_src_0 ! $VRTPSINK_L \
        rtpbin.send_rtcp_src_0 ! $VRTCPSINK_L \
    $VRTCPSRC_L ! rtpbin.recv_rtcp_sink_0 \
    $VELEM_R ! $VCAPS ! $VENC ! rtpbin.send_rtp_sink_1 \
        rtpbin.send_rtp_src_1 ! $VRTPSINK_R \
        rtpbin.send_rtcp_src_1 ! $VRTCPSINK_R \
    $VRTCPSRC_R ! rtpbin.recv_rtcp_sink_1
udpsrc port=7000 caps="application/x-rtp" ! queue ! rtppcmudepay ! mulawdec ! audioconvert ! alsasink









































