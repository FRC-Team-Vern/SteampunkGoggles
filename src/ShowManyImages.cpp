#include "ShowManyImages.h"

#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


void ShowThreeImagesInOne(const std::vector<cv::Mat>& images, cv::Mat& output) {

	//cv::Mat DispImage;

	int i;
	int m, n;
	int x, y;

	// w - Maximum number of images in a row 
	// h - Maximum number of images in a column 
	int w, h;
	cv::Size firstImageSize = images.at(0).size();

	// scale - How much we have to resize the image
	float scale;
	int max;

	// Create a new 3 channel image

	// Find the width and height of the image
	// x = img->width;
	// y = img->height;

	// Find whether height or width is greater in order to resize the image
	// max = (x > y)? x: y;

	// Find the scaling factor to resize the image
	//scale = (float) ( (float) max / size );

	// Used to Align the images
	//if( i % w == 0 && m!= 20) {
	//	m = 20;
	//	n+= 20 + size;
	//}
	
	// Set the image ROI to display the current image
	cv::Mat ImageROI0(output, cvRect(320, 0, firstImageSize.width, firstImageSize.height));
	cv::Mat ImageROI1(output, cvRect(0, 480, firstImageSize.width, firstImageSize.height));
	cv::Mat ImageROI2(output, cvRect(640, 480, firstImageSize.width, firstImageSize.height));
	
	images.at(0).copyTo(ImageROI0);
	images.at(1).copyTo(ImageROI1);
	images.at(2).copyTo(ImageROI2);
	
	// Resize the input image and copy the it to the Single Big Image
	// cvResize(img, DispImage);

	// Reset the ROI in order to display the next image
	// cvResetImageROI(DispImage);
}
