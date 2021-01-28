gst-launch-0.10 videomixer name=mixme ! ffmpegcolorspace ! jpegenc ! rtpjpegpay ! udpsink host=192.168.42.100 port=4000 sync=true \
v4l2src device=/dev/video0 ! videoscale ! ffmpegcolorspace ! video/x-raw-yuv, width=320, height=240 ! videobox border-alpha=0 ! mixme. \
v4l2src device=/dev/video1 ! videoscale ! ffmpegcolorspace ! video/x-raw-yuv, width=320, height=240 ! videobox left=-320 ! mixme.
