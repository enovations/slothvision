#include "marker.h"

void marker::generateMarker(const std::string& path) {
    cv::Mat markerImage;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::drawMarker(dictionary, 23, 600, markerImage, 1);
    cv::imwrite(path, markerImage);
}

bool marker::markerPosition(const cv::Mat& image, cv::Point2f& center) {
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::aruco::detectMarkers(image, dictionary, corners, ids);

    if (ids.size() > 0) {
        cv::Vec3f x1(corners[0][0].x, corners[0][0].y, 1.0);
        cv::Vec3f x2(corners[0][1].x, corners[0][1].y, 1.0);
        cv::Vec3f x3(corners[0][2].x, corners[0][2].y, 1.0);
        cv::Vec3f x4(corners[0][3].x, corners[0][3].y, 1.0);

        cv::Vec3f l1 = x1.cross(x3);
        cv::Vec3f l2 = x2.cross(x4);

        cv::Vec3f c = l1.cross(l2);
        c /= c[2];

        center.x = c[0];
        center.y = c[1];
        return true;
    }

    return false;
}
