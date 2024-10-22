#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    vector<Scalar> colors = {Scalar(255, 0, 0), Scalar(255, 0, 255), Scalar(0, 255, 0), Scalar(0, 0, 255), Scalar(0, 255, 255)};
    Scalar color = colors[0];
    int min_area = 1000;

    VideoCapture cam(0);
    if(!cam.isOpened()){
        cout<<"Unable to open camera!"<<endl;
        return -1;
    }

    int width = static_cast<int>(cam.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cam.get(CAP_PROP_FRAME_HEIGHT));
    Mat canvas = Mat::zeros(height,width, CV_8UC3);

    // handles light red
    Scalar lowerRed1(0, 100, 100);
    Scalar upperRed1(10, 255, 255);
    
    // handles dark red
    Scalar lowerRed2(170, 100, 100);
    Scalar upperRed2(180, 255, 255);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10));

    Point prevCenter = Point(0, 0);

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

            double area = contourArea(largestContour);

            if (area > min_area) {
                Moments M = moments(largestContour);
                int cX = int(M.m10 / M.m00);
                int cY = int(M.m01 / M.m00);

                circle(frame, Point(cX, cY), 10, Scalar(0, 0, 255), 2);

                if (cY < 65 && (20 < cX && cX < 120)) {
                    canvas = Mat::zeros(height, width, CV_8UC3);
                } else if (cY < 65) {
                    if (140 < cX && cX < 220) color = colors[0];
                    else if (240 < cX && cX < 320) color = colors[1];
                    else if (340 < cX && cX < 420) color = colors[2];
                    else if (440 < cX && cX < 520) color = colors[3];
                    else if (540 < cX && cX < 620) color = colors[4];
                }

                if(prevCenter != Point(0, 0)) {
                    line(canvas, prevCenter, Point(cX, cY), color, 2);
                }
                prevCenter = Point(cX, cY);
            } else {
                prevCenter = Point(0, 0);
            }
        }

        Mat canvas_gray, canvas_binary;
        cvtColor(canvas, canvas_gray, COLOR_BGR2GRAY);
        threshold(canvas_gray, canvas_binary, 20, 255, THRESH_BINARY_INV);
        cvtColor(canvas_binary, canvas_binary, COLOR_GRAY2BGR);

        bitwise_and(frame, canvas_binary, frame);
        bitwise_or(frame, canvas, frame);

        rectangle(frame, Point(20, 1), Point(120, 65), Scalar(122, 122, 122), -1);
        rectangle(frame, Point(140, 1), Point(220, 65), colors[0], -1);
        rectangle(frame, Point(240, 1), Point(320, 65), colors[1], -1);
        rectangle(frame, Point(340, 1), Point(420, 65), colors[2], -1);
        rectangle(frame, Point(440, 1), Point(520, 65), colors[3], -1);
        rectangle(frame, Point(540, 1), Point(620, 65), colors[4], -1);

        putText(frame, "CLEAR ALL", Point(30, 33), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "BLUE", Point(155, 33), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "VIOLET", Point(255, 33), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "GREEN", Point(355, 33), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "RED", Point(465, 33), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "YELLOW", Point(555, 33), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);

        imshow("Frame", frame);
        imshow("Canvas", canvas);

        if (waitKey(1) && 0xFF == 'q') {
            break;
        }
    }

    cam.release();
    destroyAllWindows();
    return 0;
}