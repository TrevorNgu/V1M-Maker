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
int setHeader(int mNum, int width, int height, int filterDiam, int vertSpace, int horSpace, int firstHorPos, int firstVertPos, ofstream &of);

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

    string linetxt;
    ifstream filetxt(to_convert + "image.txt");
    getline(filetxt, linetxt);
    getline(filetxt, linetxt);
    cout << linetxt << endl;

    cv::Mat image;
    image = cv::imread(to_convert + "image.pgm", 1);

    while(filetxt.good()) {
        string parse[9];
        filetxt >> parse[0] >> parse[1] >> parse[2] >> parse[3] >> parse[4] >> parse[5] >> parse[6] >> parse[7] >> parse[8];
        //cout << parse[0] << " " << parse[1] << " "  << parse[2] << " "  << parse[3] << " "  << parse[4] << " "  << parse[5] << " "  << parse[6] << " "  << parse[7] << "bep" << endl;

        if(parse[0] == "") {
            break;
        }

        int dia = stoi(parse[0]);
        int rad = dia / 2;
        int col = stoi(parse[1]);
        int row = stoi(parse[2]);
        int filter[5];

        filter[0] = stof(parse[3]);
        filter[1] = stof(parse[4]);
        filter[2] = stof(parse[5]);
        filter[3] = stof(parse[6]);
        filter[4] = stof(parse[7]);        

    cv::Point p(col, row);
    cv::circle(image, p, rad, (0, 0, 255), 1);
    int x = col;
    int y = row;
    int r = rad;
    for (int px = x - r; px <= x + r; px++) {
        for (int py = y - r; py <= y + r; py++) {
            int dx = x - px, dy = y - py;

            if (dx * dx + dy * dy <= r * r) {
            // Point is part of the circle.
                cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(px, py));
                float cx = color[0];
                float cy = color[1];
                float cz = color[2];
                color[0] = color[0] * filter[0] * filter[1] * filter[2] * filter[3] * filter[4];
                color[1] = cy;
                color[2] = color[2] * filter[0] * filter[1] * filter[2] * filter[3] * filter[4];
                cx = color[0];
                cy = color[1];
                cz = color[2] * filter[0] * filter[1] * filter[2] * filter[3] * filter[4];

                /*color[0] = color[0] * filter[1];
                color[1] = color[1] * filter[1];
                color[2] = color[2] * filter[1];
                color[0] = color[0] * filter[2];
                color[1] = color[1] * filter[2];
                color[2] = color[2] * filter[2];
                color[0] = color[0] * filter[3];
                color[1] = color[1] * filter[3];
                color[2] = color[2] * filter[3];
                color[0] = color[0] * filter[4];
                color[1] = color[1] * filter[4];
                color[2] = color[2] * filter[4];*/
                image.at<cv::Vec3b>(cv::Point(px, py)) = color;
            }
        }
    }
    }

    cout << "burh" << endl;
    /*for(int x = 0; x < image.cols; x++) {
        for(int y = 0; y < image.rows; y++) {
            cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(x, y));
            int cx = color[0];
            int cy = color[1];
            int cz = color[2];

            int scaling = createBlueYellow(cx, cy, cz);
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
            
            image.at<cv::Vec3b>(cv::Point(x, y)) = color;
        }
    }*/


    /*cv::Point p(25, 25);
    cv::circle(image, p, 16, (0, 0, 255), 1);
    int x = 25;
    int y = 25;
    int r = 16;
    for (int px = x - r; px <= x + r; px++) {
        for (int py = y - r; py <= y + r; py++) {
            int dx = x - px, dy = y - py;

            if (dx * dx + dy * dy <= r * r) {
            // Point is part of the circle.
                cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(px, py));
                int cx = color[0];
                int cy = color[1];
                int cz = color[2];
                color[0] = 255;
                color[1] = 255;
                color[2] = 0;

                image.at<cv::Vec3b>(cv::Point(px, py)) = color;

            }
        }
    }*/

    /*cv::Mat ycrcb;
    cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);

    vector<cv::Mat> channels;
    split(ycrcb, channels);

    channels[1] = 140;
    //channels[2] = 0;

    cv::Mat new_image;
    merge(channels, new_image);
    cvtColor(new_image, image, cv::COLOR_YCrCb2BGR);*/


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

int setHeader(int mNum, int width, int height, int filterDiam, int vertSpace, int horSpace, int firstHorPos, int firstVertPos, ofstream &file) {
    return 0;
}