#ifndef FASTRADIALSYMMETRYTRANSFORM_H
#define FASTRADIALSYMMETRYTRANSFORM_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>	// Sobel

// mode <- 0: Bright symmetry,
		// 1: Dark symmetry,
		// else: both
		// DEFAULT = 0
// alpha <- symmetry strictness, DEFAULT = 1
// [radiusMin, radiusMax] <- range to check for symmetry, DEFAULT = [1,5]
// gradientThreshold <- gradients with magnitude bellow this won't be considered, DEFAULT = 0
cv::Mat FastRadialSymmetryTransform(const cv::Mat &image, const int &mode = 0, const int &alpha = 1, const double &gradientThreshold = 0, const int &radiusMin = 1, const int &radiusMax = 5);

#endif FASTRADIALSYMMETRYTRANSFORM_H  // !FASTRADIALSYMMETRYTRANSFORM_H