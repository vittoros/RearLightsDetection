#include "HypothesisVerification.h"

std::vector<std::pair<int, int>> HypothesisVerification(const cv::Mat &image, const std::vector<cv::Rect> &ROI) {
	// -----------------------------------------
	// Apply SymmetryCheck on candidate areas
	//int font = cv::FONT_HERSHEY_SIMPLEX;
	std::vector<std::pair<int, int>> maxROIs;
	for (size_t i = 0; i < ROI.size(); ++i) {
		std::pair<double, double> SC = SymmetryCheck(image, ROI[i]);
		double MAE = SC.first, SSIM = SC.second;
		
		if (SSIM >= 0.2 || (MAE <= 22 && MAE >= 8)) {
			int	tempArea = ROI[i].area();
			if (!maxROIs.size()) {
				maxROIs.push_back(std::make_pair(tempArea, static_cast<int>(i)));
				continue;
			}
			
			// If two overlap, keep the largest
			std::vector<std::pair<int, int>>::iterator it;
			int max = 1;
			for (it = maxROIs.begin(); it != maxROIs.end(); ++it) {
				int overlap = (ROI[i] & ROI[it->second]).area();
				if (it->first >= tempArea && overlap) {
					max = 0;
					break;
				}
				else if (it->first < tempArea && overlap){
					it->first = tempArea;
					it->second = static_cast<int>(i);
					max = 0;
				}
			}

			if (max)
				maxROIs.push_back(std::make_pair(tempArea, static_cast<int>(i)));
		}
	}

	return maxROIs;
}