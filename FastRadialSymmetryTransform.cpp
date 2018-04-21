#include "FastRadialSymmetryTransform.h"

// mode <- 0: Bright symmetry,
		// 1: Dark symmetry,
		// else: both
		// DEFAULT = 0
// alpha <- symmetry strictness, DEFAULT = 1
// [radiusMin, radiusMax] <- range to check for symmetry, DEFAULT = [1,5]
// gradientThreshold <- gradients with magnitude bellow this won't be considered, DEFAULT = 0
cv::Mat FastRadialSymmetryTransform(const cv::Mat &image, const int &mode, const int &alpha, const double &gradientThreshold, const int &radiusMin, const int &radiusMax) {
	int rows = image.rows, cols = image.cols;
	int offset = radiusMax;
	cv::Mat output = cv::Mat::zeros(rows + 2 * offset, cols + 2 * offset, CV_64FC1);
	int BRIGHT = 0, DARK = 0;
	if (!mode) {
		BRIGHT = 1;
	}
	else if (mode == 1) {
		DARK = 1;
	}
	else {
		BRIGHT = 1;
		DARK = 1;
	}


	// Calculate gradient of image
	cv::Mat Gx(rows, cols, CV_64FC1), Gy(rows, cols, CV_64FC1);
	cv::Mat edgeMaskX = (cv::Mat_<float>(3, 1) << 1, 1, 1);
	cv::Mat edgeMaskY = (cv::Mat_<float>(1, 3) << -1, 0, 1);
	cv::Mat GradientMagnitude(rows, cols, CV_64FC1);
	double max = -1, temp;

	filter2D(image, Gx, CV_64F, edgeMaskX, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	filter2D(image, Gy, CV_64F, edgeMaskY, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	
	for (int i = 0; i < rows; ++i)
	for (int j = 0; j < cols; ++j) {
		temp = sqrt(Gx.at<double>(i, j)*Gx.at<double>(i, j) + Gy.at<double>(i, j) * Gy.at<double>(i, j));
		GradientMagnitude.at<double>(i, j) = temp;
		if (temp > max)
			max = temp;
	}

	// Calculate which pixels effects each edge point
	for (int radius = radiusMin; radius <= radiusMax; radius += 2) {
		// Initialize Orientation and Projection images to '0'
		cv::Mat O = cv::Mat::zeros(rows + 2 * offset, cols + 2 * offset, CV_64FC1);
		cv::Mat M = cv::Mat::zeros(rows + 2 * offset, cols + 2 * offset, CV_64FC1);
		cv::Mat F = cv::Mat::zeros(rows + 2 * offset, cols + 2 * offset, CV_64FC1);

		int gradientRadiusX, gradientRadiusY;
		for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j) {
			temp = GradientMagnitude.at<double>(i,j);
			if (temp > gradientThreshold * max) {
				gradientRadiusX = static_cast<int>(round(Gx.at<double>(i, j) / temp)) * radius;
				gradientRadiusY = static_cast<int>(round(Gy.at<double>(i, j) / temp)) * radius;

				if (BRIGHT) {
					O.at<double>(i + gradientRadiusX + offset, j + gradientRadiusY + offset) += 1;
					M.at<double>(i + gradientRadiusX + offset, j + gradientRadiusY + offset) += temp;
				}
				if (DARK) {
					O.at<double>(i - gradientRadiusX + offset, j - gradientRadiusY + offset) -= 1;
					M.at<double>(i - gradientRadiusX + offset, j - gradientRadiusY + offset) -= temp;
				}
			}
		}

		// Calcualte the radial symmetry contribution 'F' at spesific radius range 
		double min, max;
		cv::minMaxIdx(abs(M), &min, &max);
		M = M / max;

		O = abs(O);
		cv::minMaxIdx(O, &min, &max);
		O = O / max;
		
		pow(O, alpha, O);
		F = O.mul(M);

		// Apply Gaussian with window size of idx x idy and std = 0.25*radius
		// gaussian kernel must be odd x odd
		int idx = static_cast<int>(1.5*radius), idy = static_cast<int>(1.5*radius);
		if (idx % 2 == 0) {
			++idx;
			++idy;
		}
		double stdv = idx * 0.25;
		GaussianBlur(F, F, cv::Size(radius, radius), stdv);
		output += F;
	}


	return output(cv::Rect(offset, offset, cols, rows));
}