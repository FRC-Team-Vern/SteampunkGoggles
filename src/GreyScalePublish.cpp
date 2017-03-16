#include "networktables/NetworkTable.h"
#include "GreyScalePublish.h"
/**
 * Initializes a GreyScalePublish.
 */

namespace grip 
{

	GreyScalePublish::GreyScalePublish() 
	{
		// No op
	}
	/**
	 * Runs an iteration of the Pipeline and updates outputs.
	 *
	 * Sources need to be set before calling this method. 
	 *
	 */
	void GreyScalePublish::process(cv::Mat source0)
	{
		//Step extract color channels:
		int cvBlueChannel = 0;
		cv::extractChannel(source0, this->cvExtractBlueOutput, cvBlueChannel);
		int cvGreenChannel = 1;
		cv::extractChannel(source0, this->cvExtractGreenOutput, cvGreenChannel);
		int cvRedChannel = 2;
		cv::extractChannel(source0, this->cvExtractRedOutput, cvRedChannel);
		
		//Step CV_scaleAdd Green and Blue:
		double cvScaleaddBlueScale = -0.1;
		cv::scaleAdd(cvExtractBlueOutput, cvScaleaddBlueScale, cvExtractGreenOutput, this->cvScaleadd0Output);
		
		//Step CV_scale Green and Red:
		double cvScaleaddRedScale = -1.0;
		cv::scaleAdd(cvExtractRedOutput, cvScaleaddRedScale, cvScaleadd0Output, this->cvScaleadd1Output);
		
		//Step CV_Threshold:
		double cvThresholdThresh = 128.0;  // default Double
		double cvThresholdMaxval = 255.0;  // default Double
		int cvThresholdType = cv::THRESH_BINARY;
		cv::threshold(cvScaleadd1Output, this->cvThresholdOutput, cvThresholdThresh, cvThresholdMaxval, cvThresholdType);
					
		//Step Find_Contours:
		bool findContoursExternalOnly = false;  // default Boolean
		findContours(cvThresholdOutput, findContoursExternalOnly, this->findContoursOutput);
		
		//Step Filter_Contours:
		contourFilters.minArea = 100.0;
		contourFilters.maxArea = 50000.0;
		contourFilters.minPerimeter = 50.0;
		contourFilters.minWidth = 40.0;
		contourFilters.minHeight = 90.0;
		contourFilters.maxHeight = 1500.0;
		contourFilters.maxWidth = 200.0;
		
		filterContours(findContoursOutput, contourFilters, this->filterContoursOutput);
#if 0
		//Step find largest contour by area
		std::vector<std::vector<cv::Point>> largestContours = findLargestAreas(this->filterContoursOutput);
		if (!largestContours.empty()) 
		{
			findAndDrawCirclesAndRects(source0, largestContours);
		} else 
		{
			std::cout << "Largest contour not found:" << std::endl;
		}
		OverlayContours(source0, filterContoursOutput);
		theFinalFilter();
#endif
	}
	
	/**
	 * This method is a generated getter for the output of a Filter_Contours.
	 * @return ContoursReport output from Filter_Contours.
	 */
	std::vector<std::vector<cv::Point> >* GreyScalePublish::getfilterContoursOutput()
	{
		return &(this->filterContoursOutput);
	}

	/**
	 * Finds contours in an image.
	 *
	 * @param input The image to find contours in.
	 * @param externalOnly if only external contours are to be found.
	 * @param contours vector of contours to put contours in.
	 */
	void GreyScalePublish::findContours(cv::Mat &input, bool externalOnly, std::vector<std::vector<cv::Point> > &contours)
	{
		std::vector<cv::Vec4i> hierarchy;
		contours.clear();
		int mode = externalOnly ? cv::RETR_EXTERNAL : cv::RETR_LIST;
		int method = cv::CHAIN_APPROX_SIMPLE;
		cv::findContours(input, contours, hierarchy, mode, method);

	}

	std::vector<std::vector<cv::Point>> GreyScalePublish::findLargestAreas(std::vector<std::vector<cv::Point> > &contours)
	{
		std::vector<std::vector<cv::Point> > largestContours;

		std::sort(contours.begin(), contours.end(), [](auto& x,auto& y)->bool{return cv::contourArea(x) > cv::contourArea(y);});

		std::vector<std::vector<cv::Point>> sorted_contours;


		for (int i = 0; i < contours.size() && i < 2; ++i)
		{
			largestContours.push_back(contours.at(i));
			//rectangles.first = rectangles.second;
			//rectangles.second = cv::boundingRect(largestContours);
		}

		std::cout << "Largest areas: " << std::endl;
		for (std::vector<cv::Point> contour : largestContours)
		{
			std::cout << cv::contourArea(contour) << std::endl;
		}
		return largestContours;
	}

	void GreyScalePublish::OverlayContours(cv::Mat &input, std::vector<std::vector<cv::Point> > &contours)
	{
		cv::drawContours(input, contours, -1, cv::Scalar(0,0,255), 3);
	}

	/**
	 * Filters through contours.
	 * @param inputContours is the input vector of contours.
	 * @param contoursFilter contains parameters of contours that will be kept.
	 * @param output vector of filtered contours.
	 */
	void GreyScalePublish::filterContours(std::vector<std::vector<cv::Point> > &inputContours, ContourFilters contourFilters, std::vector<std::vector<cv::Point> > &output) 
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
		for (std::vector<cv::Point> contour: inputContours)
		{
			cv::Rect bb = boundingRect(contour);
			if (bb.width < minWidth) continue;
			if (bb.height < minHeight) continue;
			if (bb.height > maxHeight) continue;
			if (bb.width > maxWidth) continue;
			double area = cv::contourArea(contour);
			if (area < minArea) continue;
			if (area > maxArea) continue;
			if (arcLength(contour, true) < minPerimeter) continue;
			cv::convexHull(cv::Mat(contour, true), hull);
			double solid = 100 * area / cv::contourArea(hull);
			output.push_back(contour);
		}
	}

	void GreyScalePublish::findAndDrawCirclesAndRects(cv::Mat &src, std::vector<std::vector<cv::Point>> &contours)
	{
		twoLargestRectTl2Br = std::make_pair(cv::Rect(0,0,0,0),cv::Rect(0,0,0,0));
		for (std::vector<cv::Point>& contour : contours) 
		{
			cv::Point2f center;
			float radius;
			cv::minEnclosingCircle( (cv::Mat)contour, center, radius );
			cv::Scalar red = cv::Scalar(0,0,255);
			cv::Rect bb = cv::boundingRect( cv::Mat(contour));
			cv::rectangle(src, bb.tl(), bb.br(), red,5);
			cv::circle( src, center, (int)radius, red, 5);
			if(twoLargestRectTl2Br.first.tl().x > 0)
			{
				twoLargestRectTl2Br.second = bb;
			}else
			{
				twoLargestRectTl2Br.first = bb;
			}

		}
		if(contours.size() > 1)
		{
			std::cout<<"first Rect: "<<twoLargestRectTl2Br.first<<std::endl;
			std::cout<<"second Rect: "<<twoLargestRectTl2Br.second<<std::endl;
			if (twoLargestRectTl2Br.first.tl().x > twoLargestRectTl2Br.second.tl().x)
			{
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
			cv::rectangle(src, Tl, Br, cv::Scalar(0,0,255), 5);
			cv::circle(src, center, radius, cv::Scalar(0,0,255), 5);
		}
	}

	int GreyScalePublish::theFinalFilter()
	{
		if (lastXPos+300 < xPos || lastXPos-300 > xPos)
		{
			return lastXPos;
			lastXPos = xPos;
		}else
		{
			lastXPos = xPos;
			return xPos;
		}

	}

} // end grip namespace


