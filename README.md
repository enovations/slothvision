![](http://i.imgur.com/AY0lbi0.gif)
![](http://i.imgur.com/RmfC7v6.gif)
==========


<img src="http://i.imgur.com/020OPu6.png" width="400px" align="right">

# About

This is a successor to a previus project called [RiftVision](https://github.com/Sourcegasm/RiftVision).

This year there will be no quadcopter because of unfurtonate circumstances. Instead we are using a slow car that drives itself like a sloth drives a car.

# Hardware (description)

You need to have ROS supported robot in order to use this software. You can modfiy RaspberryPi/slothface.py file in order to adapt our system to your robot.

In order to use all the functions of this software, you need to have Oculus Rift 3, Joystick for control and robot with Raspberry Pi running our scripts found in RaspberryPi folder. There you can find more comphrensive guide regarding scripts on Raspberry Pi. You need 3 of them. One connects to the ROS (http://www.ros.org/) so this Raspberry needs to run ROS and be connected to the hardware. You also need to Raspebrry Pis for 3D video streaming. Each of them need to have camera connected and run our camera scripts found in RaspberryPi folder. All Raspberry Pis need to be on the same LAN. We recommend using WiFi router in AP mode so you can wirelesseley connect to your robot with our control software.

# Platforms

This project can be compiled on any platform. Use CMAKE for Unix systems and Visual Studio for Windows systems. The final software can only be executed on Windows machines.

# Installation (Windows)

* Clone this repository on your computer
* Download OpenCV 3.1 (http://opencv.org/downloads.html). Extract it somewhere on your computer. Copy contents of opencv/sources to (project root)/OpenCV/. Copy directory opencv/build into (project root)/OpenCV/ so the path to dll files is (project root)/OpenCV/build/bin/*.dll. After you do this, compile sources so all the libs are accessible.
* Make sure you have OpenCV build/bin/Release on your PATH
* Make sure to build OpenCV with aditional modules enabled (and add Aruco library) - https://github.com/opencv/opencv_contrib
* You might need to install and add to PATH Intel TBB to make OpenCV work
* Download OculusSDK 1.8 for Windows (https://developer3.oculus.com/downloads/). Extract it into OculusSDK directory so the path to the LibOVR directory is (project root)/OculusSDK/LibOVR
* Download GStreamer 1.0 (https://gstreamer.freedesktop.org/data/pkg/windows/1.8.1/ - gstreamer-1.0-devel-x86-1.8.1.msi and gstreamer-1.0-x86_64-1.8.1.msi).
* Make sure you have bin directory of GStreamer on your PATH
* Set GSTREAMER_DIR environment variable to point on gstreamer installation. In our case it was C:\gstreamer\1.0\x86_64
* Download and run OculusSetup.exe (https://www3.oculus.com/en-us/setup/) so you can setup Oculus Rift 3.
* Open Visual Studio 2015 solution file and use it to build and run the project. You need Oculus Rift 3 in order to test it.

# Installation (OSX, Linux)

Setup on Unix systems depends on the scope of modules you would like to compile. In order to compile all modules, you need:

* OpenCV 3.1 with source and compiled version
* OculusSDK 1.8
* GStreamer 1.0 installed, including devel version

Use cmake to build the project.

# Hardware (setup, diagrams)

TODO: ros module diagram, robot picture, diagram of LAN setup, ...

# How to use SlothVision software

TODO: Write manufacturers opinion.

# License

The code is distributed under the [GNU General Public License](https://github.com/Sourcegasm/slothvision/blob/master/LICENSE).  
