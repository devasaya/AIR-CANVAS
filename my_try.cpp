#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    vector<Scalar> colors = {Scalar(255, 0, 0), Scalar(255, 0, 255), Scalar(0, 255, 0), Scalar(0, 0, 255), Scalar(0, 255, 255)};
    Scalar color = colors[0];

    VideoCapture cam(0);
    if(!cam.isOpened()){
        cout<<"Unable to open camera!"<<endl;
        return -1;
    }

    int width = static_cast<int>(cam.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cam.get(CAP_PROP_FRAME_HEIGHT));
    Mat mat = Mat::zeros(height,width, CV_8UC3);

    // handles light red
    Scalar lowerRed1(0, 100, 100);
    Scalar upperRed1(10, 255, 255);
    
    // handles dark red
    Scalar lowerRed2(170, 100, 100);
    Scalar upperRed2(180, 255, 255);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10));

    // render processing loop
    while (true) {
        Mat frame;
        bool success = cam.read(frame);

        if(!success){
            cout<<"Unable to read the frame"<<endl;
            break;
        }

        flip(frame, frame, 1);

        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, lowerRed1, upperRed1, mask1);
        inRange(hsv, lowerRed2, upperRed2, mask1);
        bitwise_or(mask1, mask2, mask);
        dilate(mask, mask, kernel);

        
    }



    return 0;
}