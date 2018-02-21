#ifndef SYMMETRYCHECK_H
#define SYMMETRYCHECK_H

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"	// cvtColor()

#define SSIM_SIZE 8
#define SSIM_STEP 8

// image <- whole image
// ROI <- ROI location found from previous steps
std::pair<double, double> SymmetryCheck(const cv::Mat &image, const cv::Rect &ROI);

#endif // !SYMMETRYCHECK_H