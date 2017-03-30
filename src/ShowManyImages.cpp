#include "ShowManyImages.h"

#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


void ShowThreeImagesInOne(const std::vector<cv::Mat>& images, cv::Mat& output) {
	
	// Resize images
	cv::Size resizedImage0Size(740, 555);
	cv::Mat resizedImage0(resizedImage0Size, CV_8UC3);
	cv::Size resizedImage1_2Size(540, 405);
	cv::Mat resizedImage1(resizedImage1_2Size, CV_8UC3);
	cv::Mat resizedImage2(resizedImage1_2Size, CV_8UC3);
	
	cv::resize(images.at(0), resizedImage0, resizedImage0Size);
	cv::resize(images.at(1), resizedImage1, resizedImage1_2Size);
	cv::resize(images.at(2), resizedImage2, resizedImage1_2Size);
	
	// Set the image ROI to display the current image
	cv::Mat ImageROI0 = output(cv::Rect(270, 0, resizedImage0Size.width, resizedImage0Size.height));
	cv::Mat ImageROI1 = output(cv::Rect(0, 555, resizedImage1_2Size.width, resizedImage1_2Size.height));
	cv::Mat ImageROI2 = output(cv::Rect(740, 555, resizedImage1_2Size.width, resizedImage1_2Size.height));
	
	resizedImage0.copyTo(ImageROI0);
	resizedImage1.copyTo(ImageROI1);
	resizedImage2.copyTo(ImageROI2);
	
}
