#include "CannyEdgeDetector.h"

// Input: CV_8UC1 -> convertTo CV_32FC1
// Output: CV_32FC1 binary 0-1
cv::Mat CannyEdgeDetector(const cv::Mat &image) {
	int rows = image.rows, cols = image.cols;
	
	//-----------------------------------------------------------
	// Apply Gaussian filter 5x5
	cv::Mat blurredImage;
	image.convertTo(blurredImage, CV_32FC1);
	cv::Mat kernel = (cv::Mat_<float>(5, 5) << 1,  4,  7,  4, 1,
												4, 16, 26, 16, 4,
												7, 26, 41, 26, 7,
												4, 16, 26, 16, 4,
												1,  4,  5,  4, 1) / 273.0;

	filter2D(blurredImage, blurredImage, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	//-----------------------------------------------------------
	// Find intensity gradients of the image (Sobel)
	cv::Mat Gx(rows, cols, CV_32F), Gy(rows, cols, CV_32F);
	cv::Mat sobelX = (cv::Mat_<float>(3, 3) << 1, 0, -1,
											   2, 0, -2,
											   1, 0, -1);

	cv::Mat sobelY = (cv::Mat_<float>(3, 3) << 1, 2, 1,
											   0, 0, 0,
											  -1,-2,-1);

	filter2D(blurredImage, Gx, -1, sobelX, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	filter2D(blurredImage, Gy, -1, sobelY, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	// Use hypot to avoid overflow and underflow 
	// (+find maximum value, used in double threshold step)
	// G = (Gx^2 + Gy^2)^1/2
	cv::Mat G(rows, cols, CV_32F);
	cv::Mat theta(rows, cols, CV_16S);
	float maxValue = 0.0;
	for (int i = 0; i < rows; ++i)
	for (int j = 0; j < cols; ++j) {
		G.at<float>(i, j) = hypot(Gx.at<float>(i, j), Gy.at<float>(i, j));
		theta.at<short>(i, j) = static_cast<short>(atan2(Gy.at<float>(i, j), Gx.at<float>(i, j)) * 180 / PI);

		if (G.at<float>(i, j) > maxValue)
			maxValue = G.at<float>(i, j);
	}

	// Show image
	//G.convertTo(blurredImage, CV_8UC1);
	//cv::namedWindow("Sobel", CV_WINDOW_AUTOSIZE);
	//cv::imshow("Sobel", blurredImage);
	//-----------------------------------------------------------
	// Apply non-maximum suppression
	for (int i = 1; i < rows - 1; ++i)
	for (int j = 1; j < cols - 1; ++j) {
		if (theta.at<short>(i, j) < 0)
			theta.at<short>(i, j) += 360;

		// O degree angle group
		if (theta.at<short>(i, j) <= 22.5 || (theta.at<short>(i, j) > 157.5 && theta.at<short>(i, j) <= 202.5) || theta.at<short>(i, j) > 337.5) {
			if (G.at<float>(i, j) <= G.at<float>(i, j + 1) || G.at<float>(i, j) <= G.at<float>(i, j - 1))
				G.at<float>(i, j) = 0;
		} // 45 degree angle group
		else if ((theta.at<short>(i, j) > 22.5 && theta.at<short>(i, j) <= 67.5) || (theta.at<short>(i, j) > 202.5 && theta.at<short>(i, j) <= 247.5)) {
			if (G.at<float>(i, j) <= G.at<float>(i - 1, j + 1) || G.at<float>(i, j) <= G.at<float>(i + 1, j - 1))
				G.at<float>(i, j) = 0;
		} // 90 degree angle group
		else if ((theta.at<short>(i, j) > 67.5 && theta.at<short>(i, j) <= 112.5) || (theta.at<short>(i, j) > 247.5 && theta.at<short>(i, j) <= 292.5)) {
			if (G.at<float>(i, j) <= G.at<float>(i + 1, j) || G.at<float>(i, j) <= G.at<float>(i - 1, j))
				G.at<float>(i, j) = 0;
		}  // 135 degree angle group
		else
		if (G.at<float>(i, j) <= G.at<float>(i - 1, j - 1) || G.at<float>(i, j) <= G.at<float>(i + 1, j + 1))
			G.at<float>(i, j) = 0;
	}

	// Show image
	//G.convertTo(blurredImage, CV_8UC1);
	//cv::namedWindow("Non-maximum", CV_WINDOW_AUTOSIZE);
	//cv::imshow("Non-maximum", blurredImage);

	//-----------------------------------------------------------
	// Apply double threshold
	float high = static_cast<float>(THRESHOLD_HIGH * maxValue), low = static_cast<float>(THRESHOLD_LOW * maxValue);
	cv::Mat doubleThresMap = cv::Mat::zeros(rows, cols, CV_8UC1);
	std::vector<std::pair<int, int>> sureEdgesLocations;

	for (int i = 0; i < rows; ++i)
	for (int j = 0; j < cols; ++j) {
		if (G.at<float>(i, j) >= high) {
			doubleThresMap.at<uchar>(i, j) = 2;
			sureEdgesLocations.push_back(std::make_pair(i, j));
		}
		else if (G.at<float>(i, j) >= low)
			doubleThresMap.at<uchar>(i, j) = 1;
	}
	//-----------------------------------------------------------
	// Apply edge tracking by hysteresis
	std::deque<std::pair<int, int>> myQueue;
	int x, y;

	for (std::vector<std::pair<int, int>>::iterator it = sureEdgesLocations.begin(); it != sureEdgesLocations.end(); ++it) {
		myQueue.clear();
		myQueue.push_back(*it);

		while (!myQueue.empty()) {
			x = (myQueue.back()).first;
			y = (myQueue.back()).second;
			myQueue.pop_back();

			for (int i = x - 1; i <= x + 1; ++i)
			for (int j = y - 1; j <= y + 1; ++j) 
				if (i >= 0 && j >= 0 && i < rows && j < cols && doubleThresMap.at<uchar>(i, j) == 1) {
					doubleThresMap.at<uchar>(i, j) = 2;
					myQueue.push_back(std::make_pair(i, j));
				}

		}
	}

	// Keep only strong edges
	for (int i = 0; i < rows; ++i)
	for (int j = 0; j < cols; ++j)
	if (doubleThresMap.at<uchar>(i, j) != 2) 
		G.at<float>(i, j) = 0;
	else
		G.at<float>(i, j) = 1;

	// Show image
	//cv::namedWindow("Canny", CV_WINDOW_AUTOSIZE);
	//cv::imshow("Canny", G);

	return G;
}