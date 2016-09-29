Instructions to set up Raspbery Pi:

* enable raspi camera,
* put both scripts to /opt/ on rbpi,
* mark them as excecutable,
* assign at least 128mb ram to graphics in config -> memory split,
* edit python script: set UDP_IP to match its own ip
* add this line to `/etc/rc.local`: `usr/bin/sudo /usr/bin/python3 /opt/piton.py &`,
* reboot -> python script should start at startup

It listens on on port 8008 (it can be edited in neki.py too).
Supported commands:

* ip: <ip>  - sets ip it will send data to (default: 102.168.0.105)
* port: <port>  - sets port to send data to (default: 5000)
* run   - starts to send data via gstreamer to selected port and ip
* kill  - kills the script

Note: after run, kill must be called allways performed. Running kill always before run won't hurt.

To test if setup works, run `netcat -u <raspberry_ip> 8008`. Localy run `gst-launch-0.10 -v udpsrc port=5000 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264" ! rtph264depay ! ffdec_h264 ! ffmpegcolorspace ! autovideosink sync=false` to listen for stream from Raspberry camera.

Gstreamer is needed. On ubuntu, run:
```
    sudo apt-get install gstreamer-0.10
    sudo add-apt-repository ppa:mc3man/gstffmpeg-keep 
    sudo apt-get update
    sudo apt-get install gstreamer0.10-ffmpeg
```

