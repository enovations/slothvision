#ifndef SLOTHVISION_MARKER_H
#define SLOTHVISION_MARKER_H

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <math.h>

namespace marker {
	static const double FB = 80.8; // Focal length (in pixels) and baseline (in m)
	// FOV = atan(2592/2*0.0000014/0.0036) ~= 54, f = 2592/2/2/tan(54/2/180*pi), b = 0.0635, fb = 2592/2/2/tan(54/2/180*pi)*0.0635 ~= 80.8
    void generateMarker(const std::string& path);
    bool markerPosition(const cv::Mat& image, cv::Point2f& center, std::vector<cv::Point2f>& corners);
    double markerDistance(const cv::Point2f&, const cv::Point2f&);
}

#endif