#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <opencv2/opencv.hpp>

#include "constant.h"

using namespace std;

int createMonochrome(int red, int green, int blue);
int createRedGreen(int blue, int green, int red);
int createBlueYellow(int blue, int green, int red);
int setHeader(int mNum, int width, int height, int filterDiam, int vertSpace, int horSpace, int firstHorPos, int firstVertPos);

int main() {
    string line;
    ifstream file(to_convert + "image.pgm");
    getline(file, line);
    cout << "Version: " << line << endl;
    getline(file, line);
    cout << "Size (W x H): " << line << endl;
    getline(file, line);
    cout << "Max gray value: " << line << endl;
    string convert = converted + "image.v1m";
    ofstream to_file(convert);

    cv::Mat image;
    image = cv::imread(to_convert + "image.pgm", 1);
    cout << "burh" << endl;
    for(int x = 0; x < image.cols; x++) {
        for(int y = 0; y < image.rows; y++) {
            cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(x, y));
            int cx = color[0];
            int cy = color[1];
            int cz = color[2];

            /*int scaling = createBlueYellow(cx, cy, cz);
            color[0] += 2 * scaling; // Blue
            color[1] += scaling; // Green
            color[2] += scaling; // Red

            if(color[0] > 255) {
                color[0] = 255;
            }

            if(color[1] > 255) {
                color[1] = 255;
            }

            if(color[2] > 255) {
                color[2] = 255;
            }
            
            image.at<cv::Vec3b>(cv::Point(x, y)) = color;*/
        }
    }

    cv::Mat ycrcb;
    cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);

    vector<cv::Mat> channels;
    split(ycrcb, channels);

    channels[1] = 140;
    //channels[2] = 0;

    cv::Mat new_image;
    merge(channels, new_image);
    cvtColor(new_image, image, cv::COLOR_YCrCb2BGR);


    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);
    cv::waitKey(0);
}

int createMonochrome(int blue, int green, int red) {
    return (blue + green + red) / 3;
}

int createRedGreen(int blue, int green, int red) {
    int rg = ((red - green) / 2) + 128;
    if(rg > 255) {
        return 255;
    } else if(rg < 0) {
        return 0;
    }

    return rg;
}

int createBlueYellow(int blue, int green, int red) {
    int by = ((2 * blue - red - green) / 4) + 128;
    if(by > 255) {
        return 255;
    } else if(by < 0) {
        return 0;
    }

    return by;
}

int setHeader(int mNum, int width, int height, int filterDiam, int vertSpace, int horSpace, int firstHorPos, int firstVertPos) {
    return 0;
}