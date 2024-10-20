#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Define color palette
    vector<Scalar> colors = {Scalar(255, 0, 0), Scalar(255, 0, 255), Scalar(0, 255, 0), Scalar(0, 0, 255), Scalar(0, 255, 255)};
    Scalar color = colors[0];
    int min_area = 1000;

    // Capture from webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Error: Could not open camera" << endl;
        return -1;
    }

    int width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    Mat canvas = Mat::zeros(height, width, CV_8UC3);

    // Define HSV color range and kernel for dilation (for red color detection)
    Scalar lower_red1(0, 100, 100);
    Scalar upper_red1(10, 255, 255);
    Scalar lower_red2(170, 100, 100);
    Scalar upper_red2(180, 255, 255);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10));

    Point previous_center_point = Point(0, 0);

    while (true) {
        Mat frame;
        bool success = cap.read(frame);
        if (!success) {
            cout << "Error: Unable to read frame" << endl;
            break;
        }

        // Flip the frame horizontally
        flip(frame, frame, 1);

        // Convert to HSV and create mask
        Mat hsv, mask1, mask2, mask;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // Apply the mask for red color detection
        inRange(hsv, lower_red1, upper_red1, mask1);  // Lower red range
        inRange(hsv, lower_red2, upper_red2, mask2);  // Upper red range

        bitwise_or(mask1, mask2, mask);  // Combine both masks
        dilate(mask, mask, kernel);

        // Find contours
        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

        if (!contours.empty()) {
            // Find contour with the largest area
            vector<Point> cmax = *max_element(contours.begin(), contours.end(),
                                              [](const vector<Point>& a, const vector<Point>& b) {
                                                  return contourArea(a) < contourArea(b);
                                              });

            double area = contourArea(cmax);

            if (area > min_area) {
                // Get the center of the contour
                Moments M = moments(cmax);
                int cX = int(M.m10 / M.m00);
                int cY = int(M.m01 / M.m00);

                // Draw a circle at the center
                circle(frame, Point(cX, cY), 10, Scalar(0, 0, 255), 2);

                // Perform real-time canvas clearing when the object is in the "CLEAR ALL" area
                if (cY < 65 && (20 < cX && cX < 120)) {
                    canvas = Mat::zeros(height, width, CV_8UC3);  // Clear canvas in real-time
                } else if (cY < 65) {
                    if (140 < cX && cX < 220) {
                        color = colors[0];  // Blue
                    } else if (240 < cX && cX < 320) {
                        color = colors[1];  // Violet
                    } else if (340 < cX && cX < 420) {
                        color = colors[2];  // Green
                    } else if (440 < cX && cX < 520) {
                        color = colors[3];  // Red
                    } else if (540 < cX && cX < 620) {
                        color = colors[4];  // Yellow
                    }
                }

                // Draw lines on the canvas
                if (previous_center_point != Point(0, 0)) {
                    line(canvas, previous_center_point, Point(cX, cY), color, 2);
                }

                previous_center_point = Point(cX, cY);
            } else {
                previous_center_point = Point(0, 0);
            }
        }

        // Create binary mask for canvas
        Mat canvas_gray, canvas_binary;
        cvtColor(canvas, canvas_gray, COLOR_BGR2GRAY);
        threshold(canvas_gray, canvas_binary, 20, 255, THRESH_BINARY_INV);
        cvtColor(canvas_binary, canvas_binary, COLOR_GRAY2BGR);

        // Apply canvas to the frame
        bitwise_and(frame, canvas_binary, frame);
        bitwise_or(frame, canvas, frame);

        // Draw color rectangles and labels
        rectangle(frame, Point(20, 1), Point(120, 65), Scalar(122, 122, 122), -1);  // Clear all
        rectangle(frame, Point(140, 1), Point(220, 65), colors[0], -1);  // Blue
        rectangle(frame, Point(240, 1), Point(320, 65), colors[1], -1);  // Violet
        rectangle(frame, Point(340, 1), Point(420, 65), colors[2], -1);  // Green
        rectangle(frame, Point(440, 1), Point(520, 65), colors[3], -1);  // Red
        rectangle(frame, Point(540, 1), Point(620, 65), colors[4], -1);  // Yellow

        putText(frame, "CLEAR ALL", Point(30, 33), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "BLUE", Point(155, 33), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "VIOLET", Point(255, 33), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "GREEN", Point(355, 33), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "RED", Point(465, 33), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);
        putText(frame, "YELLOW", Point(555, 33), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2, LINE_AA);

        imshow("Frame", frame);
        imshow("Canvas", canvas);

        if (waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}