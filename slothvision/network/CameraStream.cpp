#include "CameraStream.h"
#include <sstream>
#include <Windows.h>

CameraStream::CameraStream() :
	CameraStream(5000)
{
}

CameraStream::CameraStream(int port) :
	_theThread(),
	_stop(false),
	_port(port)
{
}

CameraStream::~CameraStream()
{
	_stop = true;
	if (_theThread.joinable()) {
		_theThread.join();
	}
}

void CameraStream::setPort(int port)
{
	_port = port;
}

CameraStream & CameraStream::operator >> (cv::Mat & image)
{
	std::lock_guard<std::mutex > lock(_lock);
	image = _image.clone();
	return *this;
}

bool CameraStream::isNewFrame()
{
	if (bNewFrame)
	{
		bNewFrame = false;
		return true;
	}
	return false;
}

void CameraStream::start()
{;
	_theThread = std::thread(&CameraStream::loop, this);
}

void CameraStream::loop()
{
#ifdef ENABLE_GSTREAMER_OPENCV
	std::stringstream ss;
	ss << "udpsrc port=" << _port << " ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink";
	_cap.open(ss.str()); // TODO Blocks the thread in the case the stream is not opened!
	if ( _cap.isOpened() ) {
		while (!_stop) {
			if (_cap.grab()) {
				std::lock_guard<std::mutex > lock(_lock);
				_cap.retrieve(_image);
			}
//			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
#else
	GstElement *pipeline, *sink;
	gint width, height;
	GstSample *sample;
	gchar *descr;
	GError *error = NULL;
	gint64 duration, position;
	GstStateChangeReturn ret;
	gboolean res;
	GstMapInfo map;

	int argc = 0;
	gst_init(&argc, nullptr);

	/* create a new pipeline */
	descr =
//		g_strdup_printf("videotestsrc ! videoconvert ! videoscale ! "
//			" appsink name=sink caps=\"video/x-raw,format=BGRA\"");
	g_strdup_printf("udpsrc port=%d caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,payload=(int)96,encoding-name=(string)H264\" ! rtph264depay ! avdec_h264 ! videoconvert ! appsink name=sink caps=\"video/x-raw,format=BGRA\"", _port);
	pipeline = gst_parse_launch(descr, &error);

	if (error != NULL) {
		g_print("could not construct pipeline: %s\n", error->message);
		g_clear_error(&error);
		exit(-1);
	}

	/* get sink */
	sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

	gst_app_sink_set_max_buffers(GST_APP_SINK(sink), 1);
	gst_app_sink_set_drop(GST_APP_SINK(sink), true); // Enable stream

	/* set to PAUSED to make the first frame arrive in the sink */
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	switch (ret) {
	case GST_STATE_CHANGE_FAILURE:
		g_print("failed to play the file\n");
		return;
	case GST_STATE_CHANGE_NO_PREROLL:
		/* for live sources, we need to set the pipeline to PLAYING before we can
		* receive a buffer. We don't do that yet */
		g_print("live sources not supported yet\n");
		return;
	default:
		break;
	}
	
	/* This can block for up to 5 seconds. If your machine is really overloaded,
	* it might time out before the pipeline prerolled and we generate an error. A
	* better way is to run a mainloop and catch errors there. */
	ret = gst_element_get_state(pipeline, NULL, NULL, 5 * GST_SECOND);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print("failed to play the file\n");
		return;
	}
	GstAppSink *appsink = GST_APP_SINK(sink);

	while (!_stop) {
		sample = gst_app_sink_pull_sample(appsink);

		/*GstBuffer *buffer = gst_sample_get_buffer(sample);
		GstMapInfo map;
		gst_buffer_map(buffer, &map, GST_MAP_READ);
	*/
	///* get the duration */
	//gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration);

	//if (duration != -1)
	//	/* we have a duration, seek to 5% */
	//	position = duration * 5 / 100;
	//else
	//	/* no duration, seek to 1 second, this could EOS */
	//	position = 1 * GST_SECOND;

	///* seek to the a position in the file. Most files have a black first frame so
	//* by seeking to somewhere else we have a bigger chance of getting something
	//* more interesting. An optimisation would be to detect black images and then
	//* seek a little more */
	//gst_element_seek_simple(pipeline, GST_FORMAT_TIME,
	//(GstSeekFlags)(GST_SEEK_FLAG_KEY_UNIT | GST_SEEK_FLAG_FLUSH), position);

	//
	///* get the preroll buffer from appsink, this block untils appsink really
	//* prerolls */
	//g_signal_emit_by_name(sink, "pull-preroll", &sample, NULL);

	/* if we have a buffer now, convert it to a pixbuf. It's possible that we
	* don't have a buffer because we went EOS right away or had an error. */
		if (sample) {
			GstBuffer *buffer;
			GstCaps *caps;
			GstStructure *s;

			/* get the snapshot buffer format now. We set the caps on the appsink so
			* that it can only be an rgb buffer. The only thing we have not specified
			* on the caps is the height, which is dependant on the pixel-aspect-ratio
			* of the source material */
			caps = gst_sample_get_caps(sample);
			if (!caps) {
				g_print("could not get snapshot format\n");
				return;
			}
			s = gst_caps_get_structure(caps, 0);

			/* we need to get the final caps on the buffer to get the size */
			res = gst_structure_get_int(s, "width", &width);
			res |= gst_structure_get_int(s, "height", &height);
			if (!res) {
				g_print("could not get snapshot dimension\n");
				return;
			}

			//OutputDebugString(name.c_str());
			//OutputDebugString(" - frame received!\n");
			/* create pixmap from buffer and save, gstreamer video buffers have a stride
			* that is rounded up to the nearest multiple of 4 */
			buffer = gst_sample_get_buffer(sample);
			/* Mapping a buffer can fail (non-readable) */
			if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
				cv::Mat image(cv::Size(width, height), CV_8UC4, map.data, cv::Mat::AUTO_STEP);
				//cv::imwrite("slika.png", image);
				std::lock_guard<std::mutex > lock(_lock);
				_image = image.clone();
				gst_buffer_unmap(buffer, &map);
				bNewFrame = true;
			}
			gst_sample_unref(sample);
		}
		else {
			g_print("could not make snapshot\n");
		}
	}
	/* cleanup and exit */
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);



	//GstElement *pipeline, *source, *rtpdepay, *decode, *conv, *udpsrc;
	//GstBus *bus;
	//GstMessage *msg;
	//GstCaps *filtercaps;
	//GstStateChangeReturn ret;

	///* Initialize GStreamer */
	//int argc = 0;
	//gst_init(&argc, nullptr);

	///* Create the elements */
	//rtpdepay = gst_element_factory_make("rtph264depay", "depay");
	//decode = gst_element_factory_make("avdec_h264", "decode");
	////conv = gst_element_factory_make("videoconvert", "csp");
	//
	//udpsrc = gst_element_factory_make("udpsrc", NULL);
	//g_object_set(udpsrc, "port", 5000, NULL);
	//g_object_set(G_OBJECT(udpsrc), "caps", gst_caps_from_string("application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264"), NULL);

	///* Create the empty pipeline */
	//pipeline = gst_pipeline_new("pipeline");

	////if (!pipeline || !rtpdepay || !decode || !conv || !udpsrc) {
	////	OutputDebugString("Not all elements could be created.\n");
	////	return;
	////}

	///* Build the pipeline */
	//gst_bin_add_many(GST_BIN(pipeline), udpsrc, rtpdepay, NULL);
	//if (gst_element_link_many(udpsrc, rtpdepay, NULL) != TRUE) {
	//	OutputDebugString("Elements could not be linked.\n");
	//	gst_object_unref(pipeline);
	//	return;
	//}

	///* Modify the source's properties */
	//// g_object_set (source, "pattern", 0, NULL);

	///* Start playing */
	//ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	//if (ret == GST_STATE_CHANGE_FAILURE) {
	//	OutputDebugString("Unable to set the pipeline to the playing state.\n");
	//	gst_object_unref(pipeline);
	//	return;
	//}

	///* Wait until error or EOS */
	//bus = gst_element_get_bus(pipeline);
	//msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

	///* Parse message */
	//if (msg != NULL) {
	//	GError *err;
	//	gchar *debug_info;

	//	switch (GST_MESSAGE_TYPE(msg)) {
	//	case GST_MESSAGE_ERROR:
	//		gst_message_parse_error(msg, &err, &debug_info);
	//		g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
	//		g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
	//		g_clear_error(&err);
	//		g_free(debug_info);
	//		break;
	//	case GST_MESSAGE_EOS:
	//		g_print("End-Of-Stream reached.\n");
	//		break;
	//	default:
	//		/* We should not reach here because we only asked for ERRORs and EOS */
	//		g_printerr("Unexpected message received.\n");
	//		break;
	//	}
	//	gst_message_unref(msg);
	//}

	///* Free resources */
	//gst_object_unref(bus);
	//gst_element_set_state(pipeline, GST_STATE_NULL);
	//gst_object_unref(pipeline);
#endif
}