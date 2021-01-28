#!/bin/tcsh



set myip=192.168.42.100

set port=4000

#set width=320

#set height=240



gst-launch-0.10 v4l2src device=/dev/video0 ! 'image/jpeg,width=640,height=480, framerate=20/1' ! rtpjpegpay ! udpsink host=${myip} port=${port} sync=false &

