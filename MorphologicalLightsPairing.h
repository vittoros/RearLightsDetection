#ifndef MORPHOLOGICALLIGHTSPAIRING_H
#define MORPHOLOGICALLIGHTSPAIRING_H

#include "opencv2/imgproc/imgproc.hpp"
#include <list>

std::vector<cv::Rect> MorphologicalLightsPairing(cv::Mat binaryI, cv::Mat &cimage);

#endif MORHOLOGICALLIGHTSPAIRING_H // !MORHOLOGICALLIGHTSPAIRING_H