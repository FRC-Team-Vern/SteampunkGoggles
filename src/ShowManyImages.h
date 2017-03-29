#pragma once
#include <vector>
#include <opencv2/core/core.hpp>

void ShowThreeImagesInOne(const std::vector<cv::Mat>& images, cv::Mat& output);
