#ifndef CANNYEDGEDETECTOR_H
#define CANNYEDGEDETECTOR_H

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp" // filter2D
#include <math.h> // hypot, atan2
#include <deque>
//#include <opencv2/highgui/highgui.hpp>	//imshow

#define PI 3.14159265
#define THRESHOLD_HIGH 0.25	// Multiplication factor of max value
#define THRESHOLD_LOW 0.1

// Input: CV_8UC1 -> convertTo CV_32FC1
// Output: CV_32FC1 binary 0-1
cv::Mat CannyEdgeDetector(const cv::Mat &image);

#endif // !HORIZONTALEDGEDETECTION_H
