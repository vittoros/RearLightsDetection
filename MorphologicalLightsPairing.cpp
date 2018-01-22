#include "MorphologicalLightsPairing.h"

std::vector<cv::Rect> MorphologicalLightsPairing(cv::Mat &image, cv::Mat &cimage) {
	// --------------------------------------------------------------------------
	// For each region on binary image, find ellipse with similar second moments
	std::vector<std::vector<cv::Point> > contours;
	cv::Mat bimage = image >= 70;
	std::vector<cv::RotatedRect> found;	
	//std::vector<cv::Mat> ROI;
	std::vector<cv::Rect> ROILocation;	// contains locations of found ROIs

	findContours(bimage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	for (size_t i = 0; i < contours.size(); ++i) {
		size_t count = contours[i].size();
		if (count < 6)
			continue;

		cv::Mat pointsf;
		cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
		cv::RotatedRect box = fitEllipse(pointsf);

		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
			continue;
		//drawContours(cimage, contours, (int)i, cv::Scalar::all(255), 1, 8);

		//ellipse(cimage, box, cv::Scalar(0, 0, 255), 1, CV_AA);
		//ellipse(cimage, box.center, box.size*0.5f, box.angle, 0, 360, cv::Scalar(0, 255, 255), 1, CV_AA);

		// YOYOYO
		found.push_back(box);
		//int font = cv::FONT_HERSHEY_SIMPLEX;
		//cv::putText(cimage, std::to_string(int(box.angle)), box.center, font, 1, (255, 255, 255), 2);
	}

	// --------------------------------------------------------------------------
	// Check for aligned ellipses
	for (int i = 0; i < found.size(); ++i) {
		std::vector<std::pair<float, int>> similarityVector;
		for (int j = 0; j < found.size(); ++j) {
			if (j == i)
				continue;
			cv::RotatedRect ellipse1 = found[i], ellipse2 = found[j];

			// Find if the centers are aligned
			double angle = atan((ellipse1.center.y - ellipse2.center.y) / (ellipse1.center.x - ellipse2.center.x));
			angle *= 180.0 / 3.14159265358979323846;
			if (angle <= 5 && angle >= -5 && ellipse1.size.area() / ellipse2.size.area() <= 2.5 && ellipse2.size.area() / ellipse1.size.area() <= 2.5) {
				similarityVector.push_back(std::make_pair(abs(ellipse1.size.area() / ellipse2.size.area() - 1), j));
			}

		}

		// --------------------------------------------------------------------------
		// Find best match and define vertical boundaries
		int foundRow1, foundCol1, foundRow2, foundCol2, foundHeight, foundWidth;
		if (similarityVector.size()) {
			std::sort(similarityVector.begin(), similarityVector.end());

			int j = similarityVector.front().second;
			cv::RotatedRect ellipse1 = found[i], ellipse2 = found[j];
			int col1 = static_cast<int>(ellipse1.center.x), row1 = static_cast<int>(ellipse1.center.y);
			int col2 = static_cast<int>(ellipse2.center.x), row2 = static_cast<int>(ellipse2.center.y);

			// Define vertical boundaries of the Candidate area
			// as the extreme points of the rear lights
			if (col1 < col2) {
				foundCol1 = col1 - static_cast<int>(ellipse1.size.width/2);
				foundRow1 = row1;
				foundCol2 = col2 + static_cast<int>(ellipse2.size.width/2);
				foundRow2 = row2;
			}
			else {
				foundCol1 = col2 - static_cast<int>(ellipse2.size.width/2);
				foundRow1 = row2;
				foundCol2 = col1 + static_cast<int>(ellipse1.size.width/2);
				foundRow2 = row1;
			}

			if (foundCol1 < 0)
				foundCol1 = 0;
			if (foundCol2 >= cimage.cols)
				foundCol2 = cimage.cols - 1;
			foundWidth = foundCol2 - foundCol1;
			foundHeight = static_cast<int>(1.5*foundWidth);
			foundRow1 -= static_cast<int>(foundHeight/2);
			if (foundRow1 < 0)
				foundRow1 = 0;
			if (foundHeight + foundRow1 > cimage.rows)
				foundHeight = cimage.rows - foundRow1;
			//cv::Mat myROI(cimage, cv::Rect(foundCol1, foundRow1, foundWidth, foundHeight));
			//ROI.push_back(myROI);
			ROILocation.push_back(cv::Rect(foundCol1, foundRow1, foundWidth, foundHeight));

			// asdf
			/*drawContours(cimage, contours, (int)i, cv::Scalar::all(255), 1, 8);

			ellipse(cimage, ellipse1, cv::Scalar(0, 0, 255), 1, CV_AA);
			ellipse(cimage, ellipse1.center, ellipse1.size*0.5f, ellipse1.angle, 0, 360, cv::Scalar(0, 255, 255), -1, CV_AA);

			ellipse(cimage, ellipse2, cv::Scalar(0, 0, 255), 1, CV_AA);
			ellipse(cimage, ellipse2.center, ellipse2.size*0.5f, ellipse2.angle, 0, 360, cv::Scalar(0, 255, 255), -1, CV_AA);


			int font = cv::FONT_HERSHEY_SIMPLEX;
			cv::putText(cimage, std::to_string(int(i)), ellipse2.center, font, 1, (255, 255, 255), 2);
			int thickness = 2;
			int lineType = 8;
			cv::line(cimage, ellipse1.center, ellipse2.center, cv::Scalar(0, 0, 0), thickness, lineType);
			*/
			if (i > j) {
				found.erase(found.begin() + i);
				found.erase(found.begin() + j);
			}
			else {
				found.erase(found.begin() + j);
				found.erase(found.begin() + i);
			}
			--i;
			--j;
		}
	}

	return ROILocation;
}