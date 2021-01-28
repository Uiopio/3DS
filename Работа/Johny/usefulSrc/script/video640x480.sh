raspivid -n -w 640 -h 480 -b 4500000 -fps 60 -vf -hf -t 0 -o - |   gst-launch-1.0 -v fdsrc !  h264parse ! rtph264pay config-interval=10 pt=96 ! udpsink host=192.168.42.100 port=9000

