#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>


#include "cscore.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <thread>
#include <chrono>

#include "CameraServerCustom.h"
#include "GreyScalePublish.h"
#include "ShowManyImages.h"

#define DAEMON_NAME "vdaemon"

void daemonMe();

int main() {
	// TODO: Re-daemonize me
	// daemonMe();
	
	// TODO: Add NetworkTable back in
	NetworkTable::SetClientMode();
	NetworkTable::SetIPAddress("roborio-5461-frc.local");
	NetworkTable::Initialize();
	
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	
	std::shared_ptr<NetworkTable> table = NetworkTable::GetTable("datatable");
	
	cs::UsbCamera camera1 = frc::CameraServerCustom::GetInstance()->StartAutomaticCapture();
	// TODO: Add other cameras back in
	//cs::UsbCamera camera2 = frc::CameraServerCustom::GetInstance()->StartAutomaticCapture();
	//cs::UsbCamera camera3 = frc::CameraServerCustom::GetInstance()->StartAutomaticCapture();
	camera1.SetResolution(640, 480);
	//camera2.SetResolution(640, 480);
	//camera3.SetResolution(640, 480);
	cs::CvSink cvSink1 = frc::CameraServerCustom::GetInstance()->GetVideo(camera1); // 1181
	//cs::CvSink cvSink2 = frc::CameraServerCustom::GetInstance()->GetVideo(camera2);
	//cs::CvSink cvSink3 = frc::CameraServerCustom::GetInstance()->GetVideo(camera3);
	cs::CvSource outputStreamGreen = frc::CameraServerCustom::GetInstance()->PutVideo("Green", 640, 480); // 1182
	cs::CvSource outputStreamBlue = frc::CameraServerCustom::GetInstance()->PutVideo("Blue", 640, 480); // 1183
	cs::CvSource outputStreamRed = frc::CameraServerCustom::GetInstance()->PutVideo("Red", 640, 480); // 1184
	cs::CvSource outputStreamScaled = frc::CameraServerCustom::GetInstance()->PutVideo("Scaled Filter", 640, 480); // 1185
	cs::CvSource outputStreamThreshold = frc::CameraServerCustom::GetInstance()->PutVideo("Threshold Output", 640, 480); // 1186
	cs::CvSource outputStreamContours = frc::CameraServerCustom::GetInstance()->PutVideo("Contours Output", 640, 480); // 1187
	//cs::CvSource outputSteamCombined = frc::CameraServerCustom::GetInstance()->PutVideo("Combined", 1280, 960);
	cv::Mat source1;
	//cv::Mat source2;
	//cv::Mat source3;
	//cv::Mat finalOutput(cv::Size(2*640, 2*480), CV_8UC3);
	//std::vector<cv::Mat> sourceVec;
	
	grip::GreyScalePublish greyScalePublish(grip::GreyScalePublish::TargetType::kGear);
	
	std::cout << "Target type: " << greyScalePublish.getTargetType() << std::endl;
	
	while (true) {
		cvSink1.GrabFrame(source1);
		//cvSink2.GrabFrame(source2);
		//cvSink3.GrabFrame(source3);
		
		/*
		if (!source1.empty() && !source2.empty() && !source3.empty()) {
			sourceVec = {source1, source2, source3};
			ShowThreeImagesInOne(sourceVec, finalOutput);
			outputSteamCombined.PutFrame(finalOutput);
		}
		*/
		if (!source1.empty()) {			
			greyScalePublish.process(source1);
							
			outputStreamGreen.PutFrame(greyScalePublish.getcvExtractGreenOutput());
			outputStreamBlue.PutFrame(greyScalePublish.getcvExtractBlueOutput());
			outputStreamRed.PutFrame(greyScalePublish.getcvExtractRedOutput());
			
			outputStreamScaled.PutFrame(greyScalePublish.getcvScaleadd1Output());
			outputStreamThreshold.PutFrame(greyScalePublish.getcvThresholdOutput());
			outputStreamContours.PutFrame(source1);
			
			int xPos = greyScalePublish.GetXPos();
			std::cout << "xPos: " << xPos << std::endl;
			table->PutNumber("gearX", xPos);
		}
	}
}

void daemonMe() {
	//Set our Logging Mask and open the Log
	setlogmask(LOG_UPTO(LOG_NOTICE));
	openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

	syslog(LOG_INFO, "Entering Daemon");

	pid_t pid, sid;

	//Fork the Parent Process
	pid = fork();

	if (pid < 0) { exit(EXIT_FAILURE); }

	//We got a good pid, Close the Parent Process
	if (pid > 0) { exit(EXIT_SUCCESS); }

	//Change File Mask
	umask(0);

	//Create a new Signature Id for our child
	sid = setsid();
	if (sid < 0) { exit(EXIT_FAILURE); }

	//Change Directory
	//If we cant find the directory we exit with failure.
	if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

	//Close Standard File Descriptors
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}
