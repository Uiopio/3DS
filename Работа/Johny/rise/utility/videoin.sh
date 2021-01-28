#!/bin/sh

#VIDEO_CAPS="application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)JPEG,payload=(int)26"
VIDEO_CAPS="application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)JPEG,payload=(int)26"

#VIDEO_DEC="rtph264depay ! h264parse ! avdec_h264"
#VIDEO_DEC="rtpjpegdepay ! vaapi_jpegdec ! glupload ! glcolorconvert ! gloverlay location=calib.png ! gldownload"
VIDEO_DEC="rtpjpegdepay ! jpegdec ! videoconvert"
#VIDEO_DEC="rtpvrawdepay"

#RTPBIN_PARAMS="latency=250 buffer-mode=0 ntp-sync=true do-retransmission=false"
#RTPBIN_PARAMS="latency=250 drop-on-latency=true buffer-mode=4 ntp-sync=true do-retransmition=false ntp-time-source=3"
#RTPBIN_PARAMS="ntp-sync=true do-retransmition=false"
#RTPBIN_PARAMS="buffer-mode=4 ntp-sync=true ntp-time-source=3 do-retransmition=false"
#RTPBIN_PARAMS="latency=250 drop-on-latency=true buffer-mode=1"
RTPBIN_PARAMS=""

#DESTLEFT=192.168.1.100
#DESTRIGHT=192.168.1.100
#DEST=173.1.0.69
DEST=127.0.0.1

gst-launch-1.0  \
rtpbin name=rtpbin $RTPBIN_PARAMS \
    udpsrc caps=$VIDEO_CAPS port=5000 ! rtpbin.recv_rtp_sink_0 \
      rtpbin. ! $VIDEO_DEC ! ximagesink sync=false \
    udpsrc port=5001 ! rtpbin.recv_rtcp_sink_0 \
    rtpbin.send_rtcp_src_0 ! udpsink port=5005 host=$DEST sync=false async=false
