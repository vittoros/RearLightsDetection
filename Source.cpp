#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>  //  ostringstream
#include <iomanip>	// std::setprecision()

#include "RedColorSegmentation.h"
#include "OtsusThreshold.h"
#include "HorizontalEdgeBoundaries.h"
#include "FastRadialSymmetryTransform.h"
#include "MorphologicalLightsPairing.h"
#include "HypothesisVerification.h"

int main() {
	// Open test video
	const std::string folder_pos = "cars_markus/";
	int samples_num;

	// vectors to store all filenames of images
	std::vector<std::string> test_samples;

	// Get all filenames for testing images from folder
	cv::glob(folder_pos, test_samples, true);

	// Get number of images
	samples_num = static_cast<int>(test_samples.size());

	// Create a window for display.
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	for (int loop = 0; loop < samples_num; ++loop) {
		// Read image
		cv::Mat image = cv::imread(test_samples[loop]);

		// -----------------------------------------
		// Resize but keep ratio
		double ratio = static_cast<double>(image.rows) / image.cols;
		if (ratio < 1) {
			int x = static_cast<int>(360 * ratio);
			cv::resize(image, image, cv::Size(360, x));
		}
		else {
			int x = static_cast<int>(360 * (1.0 / ratio));
			cv::resize(image, image, cv::Size(x, 360));
		}

		// -----------------------------------------
		// Keep only red subspace of image
		cv::Mat redI = RedColorSegmentation(image);

		// -----------------------------------------
		// Apply Fast Radial Symmetry Transform
		int mode = 2;
		int alpha = 1;
		int radiusMin = 5, radiusMax = 21;
		double gradientThreshold = 0.2;
		cv::Mat myR = FastRadialSymmetryTransform(redI, mode, alpha, gradientThreshold, radiusMin, radiusMax);

		// -----------------------------------------
		// Change range to 0-255 and apply Otsu's threshold
		double min, max;
		cv::minMaxIdx(myR, &min, &max);
		cv::Mat adjMap;
		myR.convertTo(adjMap, CV_8UC1, 255 / (max - min), -min);
		OtsusThreshold(adjMap);
		
		// -----------------------------------------
		// Morphological Lights Pairing
		std::vector<cv::Rect> ROI = MorphologicalLightsPairing(adjMap, image);

		// -----------------------------------------
		// Find top and bottom boundaries of candidate vehicle
		//for (size_t i = 0; i < ROI.size(); ++i) 
			//HorizontalEdgeBoundaries(image, ROI[i]);
		
		// -----------------------------------------
		// Apply SymmetryCheck on candidate areas
		std::vector<std::pair<int, int>> maxROIs = HypothesisVerification(image, ROI);

		// -----------------------------------------
		// Draw rectangles of found vehicles
		std::vector<std::pair<int, int>>::iterator it;
		for (it = maxROIs.begin(); it != maxROIs.end(); ++it)
			cv::rectangle(image, ROI[it->second], cv::Scalar(0, 255, 255), 4);
		
		// -------------------------------------------------------------------------
		std::cout << "\rCompleted " << loop+1 << "/" << samples_num;
		cv::imshow("Display window", image);
		cv::waitKey(1);
	}

	return 0;
}