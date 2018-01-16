#ifndef REDCOLORSEGMENTATION_H
#define REDCOLORSEGMENTATION_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>	// cvtColor
#include <opencv2/contrib/contrib.hpp>	// applyColorMap

cv::Mat RedColorSegmentation(const cv::Mat &image);

#endif REDCOLORSEGMENTATION_H  // !REDCOLORSEGMENTATION_H