#include "HypothesisVerification.h"

std::vector<std::pair<int, int>> HypothesisVerification(const cv::Mat &image, const std::vector<cv::Rect> &ROI) {
	// -----------------------------------------
	// Apply SymmetryCheck on candidate areas
	//int font = cv::FONT_HERSHEY_SIMPLEX;
	std::vector<std::pair<int, int>> maxROIs;
	for (size_t i = 0; i < ROI.size(); ++i) {
		std::pair<double, double> SC = SymmetryCheck(image, ROI[i]);
		double MAE = SC.first, SSIM = SC.second;
		cv::Scalar clr(rand() % 255, rand() % 255, rand() % 255);

		/// SHIAT
		MAE *= 10;
		int temp = static_cast<int>(MAE);
		temp /= 10;
		int MAEdemical = static_cast<int>(MAE - 10 * temp);
		MAE /= 10;
		temp = static_cast<int>(MAE);

		SSIM *= 10;
		int temp2 = static_cast<int>(SSIM);
		temp2 /= 10;
		int SSIMdemical = static_cast<int>(SSIM - 10 * temp2);
		SSIM /= 10;
		temp2 = static_cast<int>(SSIM);
		/// SHIAT

		if (SSIM < 0.2 && (MAE > 29.5 || MAE < 8)) {
			continue;
			//cv::rectangle(image, ROI[i], cv::Scalar(0, 0, 255), 4);
			//cv::putText(image, std::to_string(temp) + "." + std::to_string(MAEdemical) + "-" + std::to_string(temp2) + "." + std::to_string(SSIMdemical), cv::Point(ROI[i].x, ROI[i].y), font, 1, cv::Scalar(0, 0, 255), 2);
		}
		else {
			//cv::rectangle(image, ROI[i], cv::Scalar(0, 225, 255), 4);
			//cv::putText(image, std::to_string(temp) + "." + std::to_string(MAEdemical) + "-" + std::to_string(temp2) + "." + std::to_string(SSIMdemical), cv::Point(ROI[i].x, ROI[i].y), font, 1, cv::Scalar(0, 255, 255), 2);

			int	tempArea = ROI[i].area();
			if (!maxROIs.size()) {
				maxROIs.push_back(std::make_pair(tempArea, static_cast<int>(i)));
				continue;
			}

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