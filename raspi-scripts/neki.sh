/usr/bin/raspivid -t 0 -w 1280 -h 720 -fps 25 -vf -b 1000000 -o - | /usr/bin/gst-launch-0.10 -v fdsrc ! h264parse ! rtph264pay config-interval=1 pt=96 ! udpsink host=$1 port=$2

