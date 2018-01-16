#ifndef OTSUSTHRESHOLD_H
#define OTSUSTHRESHOLD_H

#include <opencv2/highgui/highgui.hpp>
#include <vector>

#define NUMBEROFBINS 256	// range of image, grayscale => 2^8

void OtsusThreshold(cv::Mat &image);

#endif OTSUSTHRESHOLD_H  // !OTSUSTHRESHOLD_H