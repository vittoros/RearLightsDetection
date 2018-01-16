#include "OtsusThreshold.h"

void OtsusThreshold(cv::Mat &image) {
	// Calculate histogram
	int rows = image.rows, cols = image.cols;
	std::vector<double> histogram(NUMBEROFBINS);
	for (int i = 0; i < rows; ++i)
	for (int j = 0; j < cols; ++j)
		histogram[image.at<uchar>(i, j)] += 1;

	// Find total meanValue
	double meanValue = 0.0;
	for (int i = 0; i < NUMBEROFBINS; ++i) {
		histogram[i] *= 1.0 / (rows*cols);
		meanValue += i * histogram[i];
	}

	// Find threshold that maximize inter-class variance
	double cumulativeDestinyF1 = 0.0, mean1 = 0.0, mean2, sumB = 0.0, maxValue = 0.0, tempValue, threshold;
	for (int i = 0; i < NUMBEROFBINS; ++i) {
		cumulativeDestinyF1 += histogram[i];
		if (!cumulativeDestinyF1)
			continue;
		sumB += i * histogram[i];
		mean1 = sumB / cumulativeDestinyF1;
		mean2 = (meanValue - cumulativeDestinyF1*mean1) / (1.0 - cumulativeDestinyF1);

		tempValue = cumulativeDestinyF1 * (1 - cumulativeDestinyF1) * (mean1 - mean2) * (mean1 - mean2);
		if (tempValue > maxValue) {
			maxValue = tempValue;
			threshold = i;
		}
	}

	// Apply threshold
	for (int i = 0; i < rows; ++i)
	for (int j = 0; j < cols; ++j)
	if (image.at<uchar>(i, j) > threshold)
		image.at<uchar>(i, j) = 255;
	else
		image.at<uchar>(i, j) = 0;
}