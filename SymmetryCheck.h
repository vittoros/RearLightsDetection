#ifndef SYMMETRYCHECK_H
#define SYMMETRYCHECK_H

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"	// cvtColor()

#define SSIM_SIZE 8
#define SSIM_STEP 3

// image <- whole image
// ROI <- ROI location found from previous steps
void SymmetryCheck(const cv::Mat &image, cv::Rect &ROI);

#endif // !SYMMETRYCHECK_H