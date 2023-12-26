#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace cv;

struct Color32
{
	uchar red;
	uchar green;
	uchar blue;
	uchar alpha;
};

String windowName = "Unity OpenCV Object Tracking " ;
VideoCapture capture;
Rect2d bBox;


//open Camera
int StartCam()
{
	cout << "In StartCam() ";
	capture.open(0);
	if (!capture.isOpened())
	{
		cout << "StarCam() : Camera was not opened" << endl;
		return -2;
	}	
}


extern "C" int __declspec(dllexport) __stdcall Initialise(int& outputCameraWidth, int& outputCameraHeight)
{
	cout << "In Initialise()" << endl;

	// Open the stream.
	capture.open(0);
	if (!capture.isOpened())
		return -2;

	outputCameraWidth = capture.get(CAP_PROP_FRAME_WIDTH);
	outputCameraHeight = capture.get(CAP_PROP_FRAME_HEIGHT);

	return 0;

}

extern "C" void __declspec(dllexport) __stdcall StopTracking()
{
	capture.release();

}

//CSRT tracker
extern "C" void __declspec(dllexport) __stdcall Track_CSRT()
{
	cout << "In Track_CSRT()" << endl;
	Rect roi;
	Mat frame;
	Ptr<Tracker> csrtTracker = TrackerCSRT::create();

	if (!capture.isOpened())
	{
		cout << "Track_CSRT() : Camera was not opened" << endl;
	}

	capture >> frame;
	if (frame.empty())
	{
		cout << "Track_CSRT() : Frames were empty" << endl;
		return;
	}

	//select ROI
	roi = selectROI("CSRT Tracker : Select ROI", frame);

	if (roi.width == 0 || roi.height == 0)
	{
		cout << "Track_CSRT(): ROI was not selected, width and height were 0" << endl;
		return ;
	}

	// initialsie CSRT Tracker
	csrtTracker->init(frame, roi);

	//track object
	cout << "Track_CSRT(): Start tracking Process" <<endl;
	for (;;)
	{
		//get frame form the video
		capture >> frame;

		//stop if no more img data
		if (frame.rows == 0 || frame.cols == 0)
		{
			cout << "Track_CSRT(): Rows and Columns in frame are zero";
			return;
		}

		//update the tracking result
		csrtTracker->update(frame, roi);

		//draw tracked obj
		rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);

		//show the tarcked object
		imshow("CSRT Tracked Object", frame);

	}
}

//GOTURN tracker
extern "C" void __declspec(dllexport) __stdcall Track_GOTURN(bool trackFlag)
{
	cout << "In Track_GOTURN()" << endl;
	Rect roi;
	Mat frame;
	Ptr<Tracker> goturnTracker = TrackerGOTURN::create();

	if (!capture.isOpened())
	{
		cout << "Track_GOTURN() : Camera was not opened" << endl;
	}

	capture >> frame;
	if (frame.empty())
	{
		cout << "Track_GOTURN() : Frames were empty" << endl;
		return;
	}

	//select ROI
	roi = selectROI("GOTURN Tracker : Select ROI", frame);

	if (roi.width == 0 || roi.height == 0)
	{
		cout << "Track_GOTURN(): ROI was not selected, width and height were 0" << endl;
		return;
	}

	// initialsie GOTURN Tracker
	goturnTracker->init(frame, roi);

	//track object
	cout << "Track_GOTURN(): Start tracking Process" << endl;
	for (;;)
	{
		//get frame form the video
		capture >> frame;

		//stop if no more img data
		if (frame.rows == 0 || frame.cols == 0)
		{
			cout << "Track_GOTURN(): Rows and Columns in frame are zero";
			return;
		}

		//update the tracking result
		goturnTracker->update(frame, roi);

		//draw tracked obj
		rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);

		//show the tarcked object
		imshow("GOTURN Tracked Object", frame);
	}

}

//with predefined bounding box
//tracker with predefined ROI from Unity
extern "C" void __declspec(dllexport) __stdcall Track_CSRT_1()
{
	cout << "In Track_CSRT_1()" << endl;
	Rect roi;
	Mat frame;
	Rect bBox(200, 400, 100, 400);
	Ptr<Tracker> csrtTracker = TrackerCSRT::create();

	if (!capture.isOpened())
	{
		cout << "Track_CSRT() : Camera was not opened" << endl;
	}

	capture >> frame;
	if (frame.empty())
	{
		cout << "Track_CSRT() : Frames were empty" << endl;
		return;
	}

	// initialsie CSRT Tracker
	csrtTracker->init(frame, bBox);

	//track object
	cout << "Track_CSRT(): Start tracking Process" << endl;

	
	//get frame form the video
	capture >> frame;

	//stop if no more img data
	if (frame.rows == 0 || frame.cols == 0)
	{
		cout << "Track_CSRT(): Rows and Columns in frame are zero";
		return;
	}

	//update the tracking result
	csrtTracker->update(frame, bBox);

	//draw tracked obj
	rectangle(frame, bBox, Scalar(255, 0, 0), 2, 1);

	//show the tarcked object
	imshow("CSRT Tracked Object", frame);

	
}

//with webcam texture passed from Unity
extern "C" void __declspec(dllexport) __stdcall Track_CSRT_2(Color32 **rawImg, int width, int height)
{
	cout << "In Track_CSRT_2()" << endl;
	Rect roi;
	Mat frame;
	Rect bBox(200, 400, 100, 400);
	Ptr<Tracker> csrtTracker = TrackerCSRT::create();

	cout << "In Track_CSRT_2(): before while loop" << endl;
	
	//while(*rawImg)
	//{
		cout << "In Track_CSRT_2(): in while loop" << endl;
		
		Mat imgCopy;
		Mat image(height, width, CV_8SC4, *rawImg);
		
		//image.copyTo(imgCopy);
		
		// initialsie CSRT Tracker
		csrtTracker->init(image, bBox);

		//track object
		cout << "Track_CSRT(): Start tracking Process" << endl;

		//stop if no more img data
		if (image.rows == 0 || image.cols == 0)
		{
			cout << "Track_CSRT(): Rows and Columns in current image frame are zero";
			return;
		}

		//update the tracking result
		csrtTracker->update(image, bBox);

		//draw tracked obj
		rectangle(image, bBox, Scalar(255, 0, 0), 2, 1);
		

		//show the tarcked object
		//imshow("CSRTTest", imgCopy);

	//}
}
