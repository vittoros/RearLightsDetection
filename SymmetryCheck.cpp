#include "SymmetryCheck.h"

// image <- whole image
// ROI <- ROI location found from previous steps
std::pair<double, double> SymmetryCheck(const cv::Mat &image, const cv::Rect &ROI) {
	cv::Mat myROI(image, ROI);
	cvtColor(myROI, myROI, CV_BGR2GRAY);

	// Calculate Mean Absolute value 
	double MAE = 0;
	int rows = myROI.rows, cols = myROI.cols;
	for (int x = 0; x < rows; ++x)
	for (int i = 0, j = cols-1; i < j; ++i, --j)
		MAE += abs(myROI.at<uchar>(x, i) - myROI.at<uchar>(x, j));

	MAE *= 1.0 / (rows * cols);

	// Calculate Structural SIMilarity
	double SSIM = 0, k1 = 0.01, k2 = 0.03;
	int L = 255, numberOfWindows = 0;
	double c1 = k1*L*k1*L, c2 = k2*L*k2*L;
	for (int i = 0; i < rows - SSIM_SIZE; i += SSIM_STEP)
	for (int j1 = 0, j2 = cols - 1; j1 + SSIM_SIZE <= j2 - SSIM_SIZE; j1 += SSIM_STEP, j2 -= SSIM_STEP) {
		// Apply the formula on the two subwindows
		cv::Rect rect1(j1,i, SSIM_SIZE, SSIM_SIZE), rect2(j2-SSIM_SIZE+1, i, SSIM_SIZE, SSIM_SIZE);
		cv::Mat ROI1(myROI, rect1), ROI2(myROI, rect2);
		cv::Scalar mean, stdv;
		meanStdDev(ROI1, mean, stdv);
		double variance1 = stdv[0], mean1 = mean[0];
		
		meanStdDev(ROI2, mean, stdv);
		double variance2 = stdv[0], mean2 = mean[0];

		cv::Mat ROI3(rows, cols, CV_16UC1), ROI1ushort(rows, cols, CV_16UC1), ROI2ushort(rows, cols, CV_16UC1);
		cv::Mat ROI2inv;
		flip(ROI2, ROI2inv, 1);
		ROI1.convertTo(ROI1ushort, CV_16UC1);
		ROI2inv.convertTo(ROI2ushort, CV_16UC1);

		multiply(ROI1ushort, ROI2ushort, ROI3);
		mean = cv::mean(ROI3);
		double covariance = mean[0] - mean1 * mean2;

		SSIM += (2 * mean1*mean2 + c1) * (2 * covariance + c2) / ((mean1*mean1 + mean2*mean2 + c1) * (variance1*variance1 + variance2*variance2 + c2));
		++numberOfWindows;
	}

	SSIM *= 1.0 / numberOfWindows;

	return std::make_pair(MAE, SSIM);
}
