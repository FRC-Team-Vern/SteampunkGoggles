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
	private:
		cv::Mat cvExtractBlueOutput;
		cv::Mat cvExtractGreenOutput;
		cv::Mat cvExtractRedOutput;
		cv::Mat cvScaleadd0Output;
		cv::Mat cvScaleadd1Output;
		cv::Mat cvThresholdOutput;
		cv::Point center;
		int lastXPos;
		int xPos;
		std::pair<cv::Rect, cv::Rect> rectangles;
		std::vector<std::vector<cv::Point> > findContoursOutput;
		std::vector<std::vector<cv::Point> > filterContoursOutput;
		void findContours(cv::Mat &, bool , std::vector<std::vector<cv::Point> > &);
		std::pair<cv::Rect, cv::Rect> twoLargestRectTl2Br;
		
		struct ContourFilters {
			double minArea = 100.0;
			double maxArea = 50000.0;
			double minPerimeter = 50.0;
			double minWidth = 40.0;
			double minHeight = 90.0;
			double maxHeight = 1500.0;
			double maxWidth = 200.0;
		} contourFilters;

	public:
		GreyScalePublish();
		void process(cv::Mat source0);
		inline cv::Mat& getcvExtractBlueOutput() { return cvExtractBlueOutput; }
		inline cv::Mat& getcvExtractGreenOutput() { return cvExtractGreenOutput; }
		inline cv::Mat& getcvExtractRedOutput() { return cvExtractRedOutput; }
		inline cv::Mat& getcvScaleadd0Output() { return cvScaleadd0Output; }
		inline cv::Mat& getcvScaleadd1Output() { return cvScaleadd1Output; }
		inline cv::Mat& getcvThresholdOutput() { return cvThresholdOutput; }
		inline int GetXPos(){return xPos;}
		std::vector<std::vector<cv::Point>> findLargestAreas(std::vector<std::vector<cv::Point> > &contours);
		void findAndDrawCirclesAndRects(cv::Mat &, std::vector<std::vector<cv::Point>> &);
		int theFinalFilter();
		void OverlayContours(cv::Mat &, std::vector<std::vector<cv::Point> > &);
		std::vector<std::vector<cv::Point> >* getfilterContoursOutput();
		void filterContours(std::vector<std::vector<cv::Point> > &, ContourFilters, std::vector<std::vector<cv::Point> > &);
		//void filterContours(std::vector<std::vector<cv::Point> > &inputContours, double, double, double, double, double, double, double, std::vector<std::vector<cv::Point> > &output);
};


} // end namespace grip