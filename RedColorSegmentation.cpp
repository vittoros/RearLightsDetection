#include "RedColorSegmentation.h"

cv::Mat RedColorSegmentation(const cv::Mat &image) {
	// Convert RGB to L*a*b*
	cv::Mat image_out;
	cvtColor(image, image_out, CV_BGR2Lab);

	// Keep only positive of a* channel
	cv::Mat redComponent(image_out.rows, image_out.cols, CV_8UC1);
	int ch[] = {1, 0};
	mixChannels(&image_out, 1, &redComponent, 1, ch, 1);

	// Keep red subspace from red-green (positive of a* channel)
	for (int i = 0; i < redComponent.rows; ++i)
	for (int j = 0; j < redComponent.cols; ++j) {
		if (redComponent.at<uchar>(i, j) <= 127)
			redComponent.at<uchar>(i, j) = 0;
		else {
			redComponent.at<uchar>(i, j) -= 127;
			redComponent.at<uchar>(i, j) *= 2;
		}
	}

	return redComponent;
}