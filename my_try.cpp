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

        Mat hsv, mask, mask1, mask2;

        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, lowerRed1, upperRed1, mask1);
        inRange(hsv, lowerRed2, upperRed2, mask1);
        bitwise_or(mask1, mask2, mask);
        dilate(mask, mask, kernel);

        imshow("Mask: ",mask);

        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

        if(!contours.empty()) {
            // largest contour
            vector<Point> largestContour = *max_element(
                contours.begin(), contours.end(),
                [](const vector<Point> &a, const vector<Point> &b) {
                    return contourArea(a) < contourArea(b);
                }
            );

            // center of the contour
            Moments M = moments(largestContour);
            int cX = int(M.m10 / M.m00);
            int cY = int(M.m01 / M.m00);

            circle(frame, Point(cX, cY), 10, Scalar(0, 0, 255), 2);

            if (cY < 65 && (20 < cX && cX < 120)) {
                Mat canvas = Mat::zeros(height, width, CV_8UC3);
            } else if (cY < 65) {
                if (140 < cX && cX < 220) color = colors[0];
                else if (240 < cX && cX < 320) color = colors[1];
                else if (340 < cX && cX < 420) color = colors[2];
                else if (440 < cX && cX < 520) color = colors[3];
                else if (540 < cX && cX < 620) color = colors[4];
            }
            Point prev_center = ;
            if ()
        }

    }



    return 0;
}