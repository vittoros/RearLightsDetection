#include "CannyEdgeDetector.h"

// Input: CV_8UC1 -> convertTo CV_32FC1
// Output: CV_32FC1 binary 0-1
cv::Mat CannyEdgeDetector(const cv::Mat &image) {
	int rows = image.rows, cols = image.cols;
	cv::Mat blurredImage;
	
	//-----------------------------------------------------------
	// Apply Gaussian filter 5x5
	image.convertTo(blurredImage, CV_32FC1);
	cv::Size ksize1, ksize2;
	ksize1.width = 5, ksize2.width = 1;
	ksize1.height = 1, ksize2.height = 5;
	cv::GaussianBlur(blurredImage, blurredImage, ksize1, 1.4);
	cv::GaussianBlur(blurredImage, blurredImage, ksize2, 1.4);

	//-----------------------------------------------------------
	// Find intensity gradients of the image (Sobel)
	cv::Mat Gx(rows, cols, CV_32F), Gy(rows, cols, CV_32F);
	cv::Mat sobelX1 = (cv::Mat_<float>(3, 1) << 1, 2, 1);
	cv::Mat sobelX2 = (cv::Mat_<float>(1, 3) << 1, 0, -1);
	cv::Mat sobelY1 = (cv::Mat_<float>(1, 3) << 1, 2, 1);
	cv::Mat sobelY2 = (cv::Mat_<float>(3, 1) << 1, 0, -1);

	filter2D(blurredImage, Gx, -1, sobelX1, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	filter2D(Gx, Gx, -1, sobelX2, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	filter2D(blurredImage, Gy, -1, sobelY2, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	filter2D(Gy, Gy, -1, sobelY1, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	// Use hypot to avoid overflow and underflow 
	// (+find mean value, used in double threshold step)
	// G = (Gx^2 + Gy^2)^1/2
	cv::Mat G(rows, cols, CV_32F);
	cv::Mat theta(rows, cols, CV_16S);
	float meanValue = 0.0;
	for (int i = 0; i < rows; ++i)
	for (int j = 0; j < cols; ++j) {
		G.at<float>(i, j) = hypot(Gx.at<float>(i, j), Gy.at<float>(i, j));
		theta.at<short>(i, j) = static_cast<short>(atan2(Gy.at<float>(i, j), Gx.at<float>(i, j)) * 180 / PI);

		meanValue += G.at<float>(i, j);
	}
	meanValue *= static_cast<float>(1.0 / (rows * cols));

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

	//-----------------------------------------------------------
	// Apply double threshold
	float high = static_cast<float>(THRESHOLD_HIGH * meanValue), low = static_cast<float>(THRESHOLD_LOW * meanValue);
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

	return G;
}