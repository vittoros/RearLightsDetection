#include "MorphologicalLightsPairing.h"

// binaryI -> uchar (0 or 1)
// binaryI uchar => up to 255 different label (0 is background) and in first pass
// For larger images than the ones used in this application, a different data type is needed
std::vector<cv::Rect> MorphologicalLightsPairing(cv::Mat binaryI, cv::Mat &cimage) {
	// --------------------------------------------------------------------------
	// For each region on binary image, find ellipse with similar second moments
	std::vector<std::list<cv::Point> > contours;
	std::vector<cv::RotatedRect> found;
	std::vector<cv::Rect> ROILocation;
	int rows = binaryI.rows, cols = binaryI.cols;

	// --------------------------------------------------------------------------
	// Connected Component Labeling
	// Two-pass algorithm
	int label = 1, labeltemp;
	std::vector<std::vector<int>> equivalentLabels;

	// First Pass
	for (int i = 0; i < rows; ++i)
	for (int j = 0; j < cols; ++j) {
		if (binaryI.at<uchar>(i, j)) {
			labeltemp = 0;
			// Check left and upper neighbour
			if (i - 1 >= 0)
				labeltemp = binaryI.at<uchar>(i - 1, j);

			if (j - 1 >= 0 && binaryI.at<uchar>(i, j - 1)) {
				// Record equality between two labels(if exists) and keep smaller
				if (!labeltemp)
					labeltemp = binaryI.at<uchar>(i, j - 1);
				else if (labeltemp > binaryI.at<uchar>(i, j - 1)) {
					equivalentLabels[labeltemp - 1].push_back(binaryI.at<uchar>(i, j - 1));
					labeltemp = binaryI.at<uchar>(i, j - 1);
				}
				else if (labeltemp < binaryI.at<uchar>(i, j - 1))
					equivalentLabels[binaryI.at<uchar>(i, j - 1) - 1].push_back(labeltemp);
			}

			if (labeltemp) {
				// Assign neigbour label
				binaryI.at<uchar>(i, j) = labeltemp;
				contours[labeltemp - 1].push_back(cv::Point(i, j));
			}
			else {
				// Create new label
				std::list<cv::Point> myList;
				myList.push_back(cv::Point(i, j));
				contours.push_back(myList);
				equivalentLabels.push_back(std::vector<int> {});
				binaryI.at<uchar>(i, j) = label++;
			}
		}

	}

	// Second Pass
	int loop = 1;
	for (std::vector<std::vector<int>>::reverse_iterator i = equivalentLabels.rbegin(); i != equivalentLabels.rend(); ++i) {
		int temp = label - loop + 1;
		// Find smallest equivalent label
		for (std::vector<int>::iterator it = i->begin(); it != i->end(); ++it) {
			if (*it && temp > *it)
				temp = *it;
		}

		// Union the pixels of the equivalent labels
		if (temp != label - loop + 1) {
			contours[temp - 1].splice(contours[temp - 1].end(), contours[label - loop - 1]);

			// Update equality between labels
			for (std::vector<int>::iterator it = i->begin(); it != i->end(); ++it) {
				if (*it && temp < *it)
					equivalentLabels[*it - 1].push_back(temp);
			}
		}

		++loop;
	}
	
	/*
	// Update/Color the binary map
	int temp = 0;
	for (std::vector<std::list<cv::Point>>::iterator i = contours.begin(); i != contours.end(); ++i) {
		++temp;
		for (std::list<cv::Point>::iterator it = i->begin(); it != i->end(); ++it) {
			binaryI.at<uchar>(it->x, it->y) = temp;
		}
	}*/


	// --------------------------------------------------------------------------
	// Find ellipses with simillar second memonts as the regions
	for (size_t j = 0; j < contours.size(); ++j) {
		int samples = contours[j].size();
		if (samples < 10)
			continue;

		// Find Mean(X) and Mean(Y^2) for both X and Y and
		// Sum(Xi*Yi)
		double meanX = 0.0, meanX2 = 0.0, meanY = 0.0, meanY2 = 0.0;
		double covXY = 0.0;
		for (std::list<cv::Point>::iterator it = contours[j].begin(); it != contours[j].end(); ++it) {
			meanX += it->x;
			meanX2 += it->x * it->x;
			meanY += it->y;
			meanY2 += it->y * it->y;
			covXY += it->x * it->y;
		}
		meanX *= 1.0 / samples;
		meanY *= 1.0 / samples;
		meanX2 *= 1.0 / samples;
		meanY2 *= 1.0 / samples;

		// Find Variance
		double varX = meanX2 - meanX * meanX;
		double varY = meanY2 - meanY * meanY;

		// Find Covariance of X and Y
		covXY *= 1.0 / (samples);
		covXY -= meanX * meanY;

		// Construct Covariance Matrix
		cv::Mat covarianceMatrix = (cv::Mat_<double>(2, 2) << varX, covXY, covXY, varY);

		// Calculate EigenVectors and EigenValues of Covariance Matrix
		cv::Mat eigenValues, eigenVectors;
		cv::eigen(covarianceMatrix, eigenValues, eigenVectors);

		// Find ellipse representing the Covariance Matrix (== found region)
		int x = static_cast<int>(meanX), y = static_cast<int>(meanY);
		int semiMajorAxis = static_cast<int>(sqrt(eigenValues.at<double>(0, 0))), semiMinorAxis = static_cast<int>(sqrt(eigenValues.at<double>(1, 0)));
		semiMajorAxis *= 2.14593, semiMinorAxis *= 2.14593;

		// Find angle of major axis with x-axis of image
		double angle = atan2(eigenVectors.at<double>(0, 0), eigenVectors.at<double>(0, 1));
		if (angle < 0)
			angle += 6.28318530718;

		angle = 180 * angle / 3.14159265359;

		// Draw found ellipse
		//cv::ellipse(cimage, cv::Point(y, x), cv::Size(semiMajorAxis, semiMinorAxis), angle, 0, 360, cv::Scalar(255, 255, 255), 2);

		// Save found ellipse
		found.push_back(cv::RotatedRect(cv::Point(y, x), cv::Size(semiMajorAxis, semiMinorAxis), angle));
	}

	// --------------------------------------------------------------------------
	// Check for aligned ellipses
	std::vector<std::vector<int>> foundPairs(found.size());
	for (int i = 0; i < found.size(); ++i) {
		cv::RotatedRect ellipse1 = found[i];
		// --------------------------------------------
		// For case of red vehicles or single red light
		if (ellipse1.size.width * 2 >= rows / 4.0 && ellipse1.angle <= 15 && ellipse1.angle >= -15) {
			int col1 = static_cast<int>(ellipse1.center.x), row1 = static_cast<int>(ellipse1.center.y);

			// Define vertical boundaries of the Candidate area
			// as the extreme points of the rear lights
			int foundRow, foundCol1, foundCol2, foundHeight, foundWidth;
			foundWidth = static_cast<int>(0.8*ellipse1.size.width);
			foundCol1 = col1 - foundWidth;
			foundCol2 = col1 + foundWidth;
			foundRow = row1;

			if (foundCol1 < 0)
				foundCol1 = 0;
			if (foundCol2 >= cimage.cols)
				foundCol2 = cimage.cols - 1;
			
			foundWidth = foundCol2 - foundCol1;
			//foundHeight = foundWidth;
			foundHeight = static_cast<int>(0.5*foundWidth);
			foundRow -= static_cast<int>(foundHeight / 2);
			if (foundRow < 0)
				foundRow = 0;
			if (foundHeight + foundRow > cimage.rows)
				foundHeight = cimage.rows - foundRow;

			ROILocation.push_back(cv::Rect(foundCol1, foundRow, foundWidth, foundHeight));
		}

		// --------------------------------------------
		// Check for aligned ellipses
		std::vector<std::pair<float, int>> similarityVector;
		for (int j = 0; j < found.size(); ++j) {
			if (j == i)
				continue;
			cv::RotatedRect ellipse2 = found[j];

			// Find if pair already exists
			std::vector<int>::iterator it;
			for (it = foundPairs[i].begin(); it != foundPairs[i].end(); ++it)
			if (*it == j)
				break;

			if (it != foundPairs[i].end())
				continue;

			cv::Mat img = cimage.clone();
			cv::line(img, ellipse1.center, ellipse2.center, 0, 3);

			// Find if the centers are aligned
			double angle = atan((ellipse1.center.y - ellipse2.center.y) / (ellipse1.center.x - ellipse2.center.x));
			angle *= 180.0 / 3.14159265358979323846;
			double ar1 = ellipse1.size.area(), ar2 = ellipse2.size.area();
			if (angle <= 5 && angle >= -5 && ellipse1.size.area() / ellipse2.size.area() <= 3 && ellipse2.size.area() / ellipse1.size.area() <= 3) {
				// Record found pair
				foundPairs[i].push_back(j);
				foundPairs[j].push_back(i);

				int col1 = static_cast<int>(ellipse1.center.x), row1 = static_cast<int>(ellipse1.center.y);
				int col2 = static_cast<int>(ellipse2.center.x), row2 = static_cast<int>(ellipse2.center.y);

				// Define vertical boundaries of the Candidate area
				// as the extreme points of the rear lights
				int foundRow1, foundCol1, foundRow2, foundCol2, foundHeight, foundWidth;
				if (col1 < col2) {
					foundCol1 = col1 - static_cast<int>(ellipse1.size.width / 2);
					foundRow1 = row1;
					foundCol2 = col2 + static_cast<int>(ellipse2.size.width / 2);
					foundRow2 = row2;
				}
				else {
					foundCol1 = col2 - static_cast<int>(ellipse2.size.width / 2);
					foundRow1 = row2;
					foundCol2 = col1 + static_cast<int>(ellipse1.size.width / 2);
					foundRow2 = row1;
				}

				if (foundCol1 < 0)
					foundCol1 = 0;
				if (foundCol2 >= cimage.cols)
					foundCol2 = cimage.cols - 1;
				foundWidth = foundCol2 - foundCol1;
				//foundHeight = foundWidth;
				foundHeight = static_cast<int>(0.5*foundWidth);
				foundRow1 -= static_cast<int>(foundHeight / 2);
				if (foundRow1 < 0)
					foundRow1 = 0;
				if (foundHeight + foundRow1 > cimage.rows)
					foundHeight = cimage.rows - foundRow1;

				ROILocation.push_back(cv::Rect(foundCol1, foundRow1, foundWidth, foundHeight));
			}

		}
	}

	return ROILocation;
}