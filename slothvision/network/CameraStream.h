#pragma once

//#define ENABLE_GSTREAMER_OPENCV

#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <opencv2/opencv.hpp>
#ifdef ENABLE_GSTREAMER_OPENCV
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#else
#include <gst/gst.h>
#include <gst/gstbuffer.h>
#include <gst/video/video.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <gst/riff/riff-media.h>
#include <gst/pbutils/missing-plugins.h>
#include <glib.h>
#endif

class CameraStream
{
protected:
	std::thread _theThread;
	bool _stop;
	int _port;
	std::mutex _lock;
	cv::VideoCapture _cap;
	cv::Mat _image;

public:
	CameraStream();
	CameraStream(int port);
	~CameraStream();
	
	void setPort(int port);
	CameraStream& operator >> (cv::Mat& image);

	void start();

protected:
	void loop();
};