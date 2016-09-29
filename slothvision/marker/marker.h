//
// Created by Vid Drobnič on 9/29/16.
//

#ifndef SLOTHVISION_MARKER_H
#define SLOTHVISION_MARKER_H

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

namespace marker {
    void generateMarker(const std::string& path);
    bool markerPosition(const cv::Mat& image, cv::Point2f& center);
    double markerDistance(const cv::Point2f&, const cv::Point2f&);
}

#endif //SLOTHVISION_MARKER_H
