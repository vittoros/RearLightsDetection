#include "HorizontalEdgeBoundaries.h"

void HorizontalEdgeBoundaries(const cv::Mat &image) {
	// Detect edges in grayscale image of search region
	cv::Mat CannyEdgeMap = CannyEdgeDetector(image);

	// Calculate horizontal projection of edge map (+find maxValue)
	int rows = CannyEdgeMap.rows, cols = CannyEdgeMap.cols, maxValue = 0;
	std::vector<int> horizontalProjection(rows);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j)
			horizontalProjection[i] += static_cast<int>(CannyEdgeMap.at<float>(i, j));

		if (horizontalProjection[i] > maxValue)
			maxValue = horizontalProjection[i];
	}

	// Find first and last value on horizontal projection that's >= maxValue/2
	int first = -1, last;
	maxValue = static_cast<int>(maxValue/2);
	for (int i = 0; i < rows; ++i) {
		if (first < 0 && horizontalProjection[i] >= maxValue)
			first = i;

		if (horizontalProjection[i] >= maxValue)
			last = i;
	}

	// Crop candidate vehicle to complete Hypothesis Generation step
	cv::Rect myROI(0, first, cols, last-first);
	cv::Mat candidateVehicle = image(myROI);

	last = 0;	// DELETEMEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
}