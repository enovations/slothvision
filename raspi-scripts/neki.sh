/usr/bin/raspivid -t 0 -ex antishake -w 900 -h 720 -fps 30 -vf -hf -b 500000 -o - | /usr/bin/gst-launch-0.10 -v fdsrc ! h264parse ! rtph264pay config-interval=1 pt=96 ! udpsink host=$1 port=$2

