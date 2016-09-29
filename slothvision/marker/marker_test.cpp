#include "marker.h"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>

int main() {
    std::cout << "Generating marker..." << std::endl;
    marker::generateMarker("marker.jpg");
    std::cout << "Marker generated." << std::endl << "Starting searching for marker..." << std::endl;

    cv::namedWindow("Marker Test", CV_WINDOW_AUTOSIZE);

    cv::VideoCapture inputVideo;
    inputVideo.open(0);

    while (inputVideo.grab()) {
        cv::Mat image;
        inputVideo.retrieve(image);

        cv::Point2f markerPosition;
        if (marker::markerPosition(image, markerPosition)) {
            cv::circle(image, cv::Point2f(markerPosition.x, markerPosition.y), 10, cv::Scalar(255, 0, 255), -1);
        }

        cv::imshow("Marker Test", image);

        char key = (char) cv::waitKey(1);
        if (key == 27)
            break;
    }

    return 0;
}