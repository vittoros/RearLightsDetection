#ifndef HYPOTHESISVERIFICATION_H
#define HYPOTHESISVERIFICATION_H

#include "SymmetryCheck.h"

std::vector<std::pair<int, int>> HypothesisVerification(const cv::Mat &image, const std::vector<cv::Rect> &ROI);

#endif // !HYPOTHESISVERIFICATION_H
