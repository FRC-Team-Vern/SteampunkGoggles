#include "networktables/NetworkTable.h"
#include "GreyScalePublish.h"
/**
 * Initializes a GreyScalePublish.
 */

namespace grip 
{

	GreyScalePublish::GreyScalePublish(TargetType targetType)
		:targetType(targetType) 
	{
		switch(targetType) {
			case kGear:
			this->contourFilters = this->gearFilters;
			break;
			case kBoiler:
			this->contourFilters = this->boilerFilters;
			break;
			default:
			this->contourFilters = this->gearFilters;
		}
		imageXSize = 640;
		shiftedCenter = (int) (640 * 0.5);
		int erosion_type = cv::MORPH_RECT;
		int erosion_size = 10;
		dilation_erode_element = cv::getStructuringElement(erosion_type, 
			cv::Size( 2*erosion_size + 1, 2*erosion_size + 1),
			cv::Point( erosion_size, erosion_size)
		);
	}
	/**
	 * Runs an iteration of the Pipeline and updates outputs.
	 *
	 * Sources need to be set before calling this method. 
	 *
	 */
	void GreyScalePublish::process(cv::Mat& source0)
	{
		this->source0 = source0;
		//Step extract color channels:
		int cvBlueChannel = 0;
		cv::extractChannel(source0, this->cvExtractBlueOutput, cvBlueChannel);
		int cvGreenChannel = 1;
		cv::extractChannel(source0, this->cvExtractGreenOutput, cvGreenChannel);
		int cvRedChannel = 2;
		cv::extractChannel(source0, this->cvExtractRedOutput, cvRedChannel);
		
		//Step CV_scaleAdd Green and Blue:
		cv::scaleAdd(cvExtractBlueOutput, cvScaleaddBlueScale, cvExtractGreenOutput, this->cvScaleadd0Output);
		
		//Step CV_scale Green and Red:
		cv::scaleAdd(cvExtractRedOutput, cvScaleaddRedScale, cvScaleadd0Output, this->cvScaleadd1Output);
		
		//Step CV_Threshold:
		threshold();
		
		//Step dilate then erode to remove holes inside of contours
		dilate_erode();
		
		//Step Find_Contours:
		findContours(cvThresholdOutput, this->findContoursOutput);
		OverlayContours(source0, findContoursOutput);
				
		filterContours(findContoursOutput, this->filterContoursOutput);
		OverlayContours(source0, filterContoursOutput, cv::Scalar(0, 255, 0));
	
		//Step find largest contours by area
		GreyScalePublish::Contours largestContours;
		findLargestContoursByArea(this->filterContoursOutput, largestContours);
		if (!largestContours.empty()) {
			findAndDrawCirclesAndRects(source0, largestContours);
		} else {
			std::cout << "Largest contour not found:" << std::endl;
		}
		
		// Eliminates big sudden jumps in data usually caused by dropped images
		TheFinalFilter();

	}
	
	void GreyScalePublish::threshold() {
		cv::threshold(cvScaleadd1Output, cvThresholdOutput, cvThresholdThresh, cvThresholdMaxval, cvThresholdType);
	}
	
	void GreyScalePublish::dilate_erode() {
		cv::dilate(cvThresholdOutput, cvThresholdOutput, dilation_erode_element);
		cv::erode(cvThresholdOutput, cvThresholdOutput, dilation_erode_element);
	}

	/**
	 * Finds contours in an image.
	 *
	 * @param input The image to find contours in.
	 * @param externalOnly if only external contours are to be found.
	 * @param contours vector of contours to put contours in.
	 */
	void GreyScalePublish::findContours(cv::Mat &input, Contours& contours)
	{
		std::vector<cv::Vec4i> hierarchy;
		contours.clear();
		int mode = findContoursExternalOnly ? cv::RETR_EXTERNAL : cv::RETR_LIST;
		int method = cv::CHAIN_APPROX_SIMPLE;
		cv::findContours(input, contours, hierarchy, mode, method);
	}

	void GreyScalePublish::findLargestContoursByArea(Contours& contours, Contours& largestContours) {
		
		std::sort(contours.begin(), contours.end(), [](auto& x,auto& y)->bool{return cv::contourArea(x) > cv::contourArea(y);});

		std::vector<std::vector<cv::Point>> sorted_contours;


		for (unsigned int i = 0; i < contours.size() && i < 2; ++i)
		{
			largestContours.push_back(contours.at(i));
		}

		std::cout << "Largest areas: " << std::endl;
		for (std::vector<cv::Point> contour : largestContours)
		{
			std::cout << cv::contourArea(contour) << std::endl;
		}
	}

	void GreyScalePublish::OverlayContours(cv::Mat &input, Contours& contours, cv::Scalar color)
	{
		cv::drawContours(input, contours, -1, color, 3);
	}

	/**
	 * Filters through contours.
	 * @param inputContours is the input vector of contours.
	 * @param contoursFilter contains parameters of contours that will be kept.
	 * @param output vector of filtered contours.
	 */
	void GreyScalePublish::filterContours(const Contours& inputContours, Contours& output) 
	{
		double minArea = contourFilters.minArea;
		double maxArea = contourFilters.maxArea;
		double minPerimeter = contourFilters.minPerimeter;
		double minWidth = contourFilters.minWidth;
		double minHeight = contourFilters.minHeight;
		double maxHeight = contourFilters.maxHeight;
		double maxWidth = contourFilters.maxWidth;
		
		std::vector<cv::Point> hull;
		output.clear();
		
		for (const std::vector<cv::Point>& contour: inputContours)
		{
			cv::Rect bb = boundingRect(contour);
			std::cout << "Current bb: " << bb << std::endl;
			if (bb.width < minWidth) {
				 std::cout << "Rejected at minWidth: " << minWidth << std::endl;
				 continue;
			}	 
			if (bb.height < minHeight) {
				 std::cout << "Rejected at minHeight: " << minHeight << std::endl;
				 continue;
			}
			if (bb.height > maxHeight) {
				 std::cout << "Rejected at maxHeight: " << maxHeight << std::endl;
				 continue;
			}
			if (bb.width > maxWidth) {
				 std::cout << "Rejected at maxWidth: " << maxWidth << std::endl;
				 continue;
			}
			double area = cv::contourArea(contour);
			if (area < minArea) continue;
			if (area > maxArea) continue;
			if (arcLength(contour, true) < minPerimeter) continue;
			// cv::convexHull(cv::Mat(contour, true), hull);
			output.push_back(contour);
		}
		
		if (output.empty()) {
			std::cout << "No contours remaining: " << std::endl;
		} else {
			std::cout << "Contours found!" << std::endl;	
		}
	}
	
	void GreyScalePublish::calcXPos(int centerPoint) {
			xPos = centerPoint - shiftedCenter;
	}

	void GreyScalePublish::findAndDrawCirclesAndRects(cv::Mat& src, Contours& contours) {

		twoLargestRectTl2Br = std::make_pair(cv::Rect(0,0,0,0),cv::Rect(0,0,0,0));
		
		for (std::vector<cv::Point>& contour : contours) {
			//cv::Point2f center;
			//float radius;
			// cv::minEnclosingCircle( (cv::Mat)contour, center, radius );
			
			cv::Rect bb = cv::boundingRect( cv::Mat(contour));
			cv::rectangle(src, bb.tl(), bb.br(), Blue, 2);
			// cv::circle( src, center, (int)radius, Blue, 2);
			
			if(twoLargestRectTl2Br.first.tl().x > 0) {
				twoLargestRectTl2Br.second = bb;
			} else {
				twoLargestRectTl2Br.first = bb;
			}

		}
		
		if(contours.size() > 1) {
			std::cout<<"first Rect: "<<twoLargestRectTl2Br.first<<std::endl;
			std::cout<<"second Rect: "<<twoLargestRectTl2Br.second<<std::endl;
			
			if (twoLargestRectTl2Br.first.tl().x > twoLargestRectTl2Br.second.tl().x) {
				const cv::Rect secondRect = twoLargestRectTl2Br.second;
				const cv::Rect firstRect = twoLargestRectTl2Br.first;
				twoLargestRectTl2Br = std::make_pair(secondRect, firstRect);
			}
			cv::Point Tl = twoLargestRectTl2Br.first.tl();
			cv::Point Br = twoLargestRectTl2Br.second.br();
			std::cout<<"Tl:"<<Tl<<std::endl;
			std::cout<<"Br:"<<Br<<std::endl;
			cv::Point center = (Tl + Br)*.5;
			std::cout<<"center:"<<center<<std::endl;
			const int radius = ((Br.x - Tl.x)*.5);
			std::cout<<"radius:"<<radius<<std::endl;
			cv::rectangle(src, Tl, Br, Red, 2);
			cv::circle(src, center, radius, Red, 2);
			calcXPos(center.x);
		}
	}

	void GreyScalePublish::TheFinalFilter() {
		if (lastXPos+200 > xPos && lastXPos-200 < xPos) {
			lastXPos = xPos;
		} else {
			xPos = lastXPos;
		}
	}

} // end grip namespace


