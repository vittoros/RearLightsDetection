#ifndef HORIZONTALEDGEBOUNDARIES_H
#define HORIZONTALEDGEBOUNDARIES_H

#include "CannyEdgeDetector.h"
#include <opencv2/core/core.hpp>
#include <vector>

// image <- whole image
// ROI <- ROI location found from previous steps
void HorizontalEdgeBoundaries(const cv::Mat &image, cv::Rect &ROI);

#endif // !HORIZONTALEDGEDETECTION_H
