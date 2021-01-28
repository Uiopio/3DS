#!/bin/sh
#run CAN bus
#sudo ip link set can0 up type can bitrate 500000
#starting demon can2net
/home/pi/can2net /home/pi/libcan_callback.so &
/home/pi/shutdown.py
# video
#/home/pi/deamon_gst /home/pi/video/video2HD.sh 4000 &
#/home/pi/video/rasp.sh &
#/home/pi/gstreamer/test/lupoglazWithAudio/lupoglazVideo.sh &
#/home/pi/gstreamer/test/lupoglazWithAudio/lupoglazAudio.sh
