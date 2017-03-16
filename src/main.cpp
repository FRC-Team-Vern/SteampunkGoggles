
#include "cscore.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <thread>
#include <chrono>

#include "CameraServerCustom.h"
#include "GreyScalePublish.h"

int main() {
	NetworkTable::SetClientMode();
	NetworkTable::SetIPAddress("roborio-5461-frc.local");
	NetworkTable::Initialize();
	
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	
	cs::UsbCamera camera1 = frc::CameraServerCustom::GetInstance()->StartAutomaticCapture();
	//cs::UsbCamera camera2 = frc::CameraServerCustom::GetInstance()->StartAutomaticCapture();
	camera1.SetResolution(640, 480);
	//camera2.SetResolution(640, 480);
	cs::CvSink cvSink1 = frc::CameraServerCustom::GetInstance()->GetVideo();
	//cs::CvSink cvSink2 = frc::CameraServerCustom::GetInstance()->GetVideo();
	cs::CvSource outputStreamGreen = frc::CameraServerCustom::GetInstance()->PutVideo("Green", 640, 480);
	cs::CvSource outputStreamBlue = frc::CameraServerCustom::GetInstance()->PutVideo("Blue", 640, 480);
	cs::CvSource outputStreamRed = frc::CameraServerCustom::GetInstance()->PutVideo("Red", 640, 480);
	cs::CvSource outputStreamScaled = frc::CameraServerCustom::GetInstance()->PutVideo("Scaled Filter", 640, 480);
	cs::CvSource outputStreamThreshold = frc::CameraServerCustom::GetInstance()->PutVideo("Threshold Output", 640, 480);
	cv::Mat source1;
	
	grip::GreyScalePublish greyScalePublish;
	
	while (true) {
		cvSink1.GrabFrame(source1);
		//cvSink2.GrabFrame(source2);
		
		if (!source1.empty()) {
			greyScalePublish.process(source1);
							
			outputStreamGreen.PutFrame(greyScalePublish.getcvExtractGreenOutput());
			outputStreamBlue.PutFrame(greyScalePublish.getcvExtractBlueOutput());
			outputStreamRed.PutFrame(greyScalePublish.getcvExtractRedOutput());
			
			outputStreamScaled.PutFrame(greyScalePublish.getcvScaleadd1Output());
			outputStreamThreshold.PutFrame(greyScalePublish.getcvThresholdOutput());
		}
	}
}
