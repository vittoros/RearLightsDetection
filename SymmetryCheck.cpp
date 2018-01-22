#include "SymmetryCheck.h"

// image <- whole image
// ROI <- ROI location found from previous steps
void SymmetryCheck(const cv::Mat &image, cv::Rect &ROI) {
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
	double SIM = 0;
}