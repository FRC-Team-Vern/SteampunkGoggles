#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include <memory>


namespace grip {

/**
* GreyScalePublish class.
* 
* An OpenCV pipeline generated by GRIP.
*/
class GreyScalePublish {
	public:
		// Public types
		enum TargetType { kGear, kBoiler };
		typedef std::vector<std::vector<cv::Point>> Contours;
		
		// Constructors
		GreyScalePublish(TargetType);
		
		// Methods
		inline TargetType getTargetType() { return this->targetType; }
		void process(cv::Mat&);
		inline cv::Mat& getcvExtractBlueOutput() { return cvExtractBlueOutput; }
		inline cv::Mat& getcvExtractGreenOutput() { return cvExtractGreenOutput; }
		inline cv::Mat& getcvExtractRedOutput() { return cvExtractRedOutput; }
		inline cv::Mat& getcvScaleadd0Output() { return cvScaleadd0Output; }
		inline cv::Mat& getcvScaleadd1Output() { return cvScaleadd1Output; }
		inline cv::Mat& getcvThresholdOutput() { return cvThresholdOutput; }
		inline Contours getfilterContoursOutput() { return filterContoursOutput; }
		inline cv::Mat& getSource0() { return source0; }
				
		void findAndDrawCirclesAndRects(cv::Mat &, Contours &);
		void OverlayContours(cv::Mat &, Contours&, cv::Scalar color = cv::Scalar(0,0,255));
		//void filterContours(std::vector<std::vector<cv::Point> > &inputContours, double, double, double, double, double, double, double, std::vector<std::vector<cv::Point> > &output);
		inline int GetXPos(){return xPos;}
	
	private:
		// Private types
		struct ContourFilters {
			double minArea;
			double maxArea;
			double minPerimeter;
			double minWidth;
			double minHeight;
			double maxHeight;
			double maxWidth;
		};
		
		// Fields
		struct ContourFilters gearFilters = {1.0, 50000.0, 50.0, 20.0, 40.0, 1500.0, 200.0};
		struct ContourFilters boilerFilters = {100.0, 50000.0, 50.0, 40.0, 90.0, 1500.0, 200.0};
		struct ContourFilters contourFilters;	
		cv::Scalar Blue = cv::Scalar(255,0,0);
		cv::Scalar Green = cv::Scalar(0,255,0);
		cv::Scalar Red = cv::Scalar(0,0,255);
		int imageXSize;
		int shiftedCenter;
	
		TargetType targetType;
		cv::Mat source0;
		
		cv::Mat cvExtractBlueOutput;
		cv::Mat cvExtractGreenOutput;
		cv::Mat cvExtractRedOutput;
		double cvScaleaddBlueScale = -0.1;
		cv::Mat cvScaleadd0Output;
		double cvScaleaddRedScale = -0.7;
		cv::Mat cvScaleadd1Output;
		double cvThresholdThresh = 75.0;
		double cvThresholdMaxval = 255.0;
		int cvThresholdType = cv::THRESH_BINARY;
		bool findContoursExternalOnly = false;  // default Boolean
		cv::Mat cvThresholdOutput;
		cv::Mat dilation_erode_element;
		cv::Point center;
		bool initXPos = true;
		int lastXPos = 0;
		int xPos;
		std::pair<cv::Rect, cv::Rect> rectangles;
		Contours findContoursOutput;
		Contours filterContoursOutput;
		std::pair<cv::Rect, cv::Rect> twoLargestRectTl2Br;
		
		// Methods
		void threshold();
		void dilate_erode();
		void findContours(cv::Mat&, Contours&);
		void filterContours(const Contours&, Contours&);
		void findLargestContoursByArea(Contours&, Contours&);
		void calcXPos(int);
		void TheFinalFilter();
};


} // end namespace grip
