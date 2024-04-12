#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <math.h>
#include <map>
#include <opencv2/opencv.hpp>

#include "constant.h"
#include "filter.cpp"

using namespace std;

int createMonochrome(int red, int green, int blue);
int createRedGreen(int blue, int green, int red);
int createBlueYellow(int blue, int green, int red);
int setHeader(string mNum, int width, int height, int filterDiam, int vertSpace, int horSpace, int firstHorPos, int firstVertPos, ofstream &of);
void setFilters(ifstream &file, ifstream &filetxt, cv::Mat &image, int* headers);
float bump( float x);

map<int, vector<Filter>> pair_filters;
map<int, vector<int>> col_row;
vector<int> col_vals;

int main(int argc, char *argv[]) {
    string command = argv[1];

    // Parse ALL files
    /*transform(command.begin(), command.end(), command.begin(), ::tolower); 

    if(command == "all") {

    }*/

    ifstream file(to_convert + command + ".pgm");
    ifstream filetxt(to_convert + command + ".txt");

    if(!file.good() || !filetxt.good()) {
        cerr << "Error! One of the files does not exist." << endl;
        exit(-1);
    }

    // Disgusting parsing of the PGM file
    string line;
    getline(file, line);
    cout << "Version: " << line << endl;
    getline(file, line);
    cout << "Size (W x H): " << line << endl;
    getline(file, line);
    cout << "Max gray value: " << line << endl;

    // Creating the v1m file format
    string convert = converted + "image.V1M";
    ofstream to_file(convert);

    // More disgusting parsing of the first 2 lines from the QDED text file
    string linetxt;
    getline(filetxt, linetxt);
    getline(filetxt, linetxt);

    // Gets the image stored into the image var
    cv::Mat image;
    image = cv::imread(to_convert + command + ".pgm", 1);

    int headers[3];
    setFilters(file, filetxt, image, headers);
    cout << headers[0] << " " << headers[1] << " " << headers[2] << endl;
    setHeader("V4", image.rows, image.cols, headers[0], row_space, col_space, headers[1], headers[2], to_file);

    for(int i = 0; i < 31; i++) {
        for(int j = 0; j < 55; j++) {
            int8_t first = pair_filters[i].at(j).firstFil + 128;
            int8_t second = pair_filters[i].at(j).secFil + 128;
            int8_t third = pair_filters[i].at(j).thirdFil + 128;
            int8_t fourth = pair_filters[i].at(j).fourthFil + 128;
            int8_t fifth = pair_filters[i].at(j).fifthFil + 128;
            //cout << pair_filters[i].at(j).firstFil << endl;

            if(i == 0) {
                            cout << pair_filters[i].at(j).firstFil << endl;

                //cout << "balls" << first << " " << second << " " << third << " " << fourth << " " << fifth << endl;
            }

            to_file << first << second << third << fourth << fifth;
            
        }
    }
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

int setHeader(string mNum, int width, int height, int filterDiam, int vertSpace, int horSpace, int firstHorPos, int firstVertPos, ofstream &file) {
    file << mNum << "\n";
    file << width << "\n";
    file << height << "\n";
    file << 8 << "\n";
    file << filterDiam << "\n";
    file << vertSpace << "\n";
    file << horSpace << "\n";
    file << firstHorPos << "\n";
    file << firstVertPos << "\n";
    
    return 0;
}

void setFilters(ifstream &file, ifstream &filetxt, cv::Mat &image, int* headers) {
    bool first = true;
    vector<vector<Filter>> filters;
    while(filetxt.good()) {
        string parse[9];
        filetxt >> parse[0] >> parse[1] >> parse[2] >> parse[3] >> parse[4] >> parse[5] >> parse[6] >> parse[7] >> parse[8];
        //cout << parse[0] << " " << parse[1] << " "  << parse[2] << " "  << parse[3] << " "  << parse[4] << " "  << parse[5] << " "  << parse[6] << " "  << parse[7] << "bep" << endl;

        // Disgusting way to check whether we've hit the end of the file
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

        if(first == true) {
            headers[0] = dia;
            headers[1] = col;
            headers[2] = row;
            first = false;
        }

        cv::Point p(col, row);
        cv::circle(image, p, rad, (0, 0, 255), 1);
        int x = col;
        int y = row;
        int r = rad;
        float firstFilter = 0;
        float secondFilter = 0;
        float thirdFilter = 0;
        float fourthFilter = 0;
        float fifthFilter = 0;

        int pointx = col / col_dif;
        int pointy = row / row_dif;
        cout << pointx << ", " << pointy << endl;

        for (int px = x - r; px <= x + r; px++) {
            for (int py = y - r; py <= y + r; py++) {
                int dx = x - px, dy = y - py;
                
                if (dx * dx + dy * dy <= r * r) {
                // Point is part of the circle.
                    cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(px, py));
                    float cx = color[0];
                    firstFilter += color[0] * filter[0];
                    secondFilter += color[0] * filter[1];
                    thirdFilter += color[0] * filter[2];
                    fourthFilter += color[0] * filter[3];
                    fifthFilter += color[0] * filter[4];

                    //color[0] = color[0] * filter[0] * filter[1] * filter[2] * filter[3] * filter[4];
                    //color[1] = cy;
                    //color[2] = color[2] * filter[0] * filter[1] * filter[2] * filter[3] * filter[4];
                    
                    /*cx = color[0];
                    cy = color[1];
                    cz = color[2] * filter[0] * filter[1] * filter[2] * filter[3] * filter[4];*/

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
        firstFilter /= divide_val;
        secondFilter /= divide_val;
        thirdFilter /= divide_val;
        fourthFilter /= divide_val;
        fifthFilter /= divide_val;

        ceil(firstFilter);
        ceil(secondFilter);
        ceil(thirdFilter);
        ceil(fourthFilter);
        ceil(fifthFilter);

        Filter fil;
        fil.firstFil = firstFilter;
        fil.secFil = secondFilter;
        fil.thirdFil = thirdFilter;
        fil.fourthFil = fourthFilter;
        fil.fifthFil = fifthFilter;
        fil.debug = to_string(pointx) + " " + to_string(pointy);
        fil.debugpix = to_string(col) + " " + to_string(row);

        /*firstFilter += 128;
        secondFilter += 128;
        thirdFilter += 128;
        fourthFilter += 128;
        fifthFilter += 128;*/

        /*if(firstFilter > 128 || firstFilter < -128) {
            cout << "firstFilter: " << firstFilter << endl;
        }

        if(secondFilter > 128 || secondFilter < -128) {
            cout << "secondFilter: " << secondFilter << endl;
        }

        if(thirdFilter > 128 || thirdFilter < -128) {
            cout << "thirdFilter: " << thirdFilter << endl;
        }

        if(fourthFilter > 128 || fourthFilter < -128) {
            cout << "fourthFilter: " << fourthFilter << endl;
        }

        if(fifthFilter > 128 || fifthFilter < -128) {
            cout << "fifthFilter: " << fifthFilter << endl;
        }*/

        if(true
            //find_if(pair_filters[pointy].begin(), pair_filters[pointy].begin())
            ) {
            pair_filters[pointy].push_back(fil);
        }

        if(col == 600 && row == 396) {
            cv::circle(image, p, rad, (0, 255, 255), -1);
        }

        //cout << firstFilter << " " << secondFilter << " " << thirdFilter << " " << fourthFilter << " " << fifthFilter << endl;
    }
}

float bump( float x)
/*
  This function generates a bump function mapping the interval
  [-sqrt(LIM_SQ),sqrt(LIM_SQ)] onto [0,1].
  If called with a line such as "y=bump(x)", where x
  has been specified, it will automatically return the corresponding value.
  bump(x) is supported on the interval specified by LIM_SQ, clamped to 0
  outside it, and normalized to peak at y(x) = 1.0 when x = 0.0

*/
{
    float xx;

    xx = x * x;
    if (xx >= (float) (0.203636 - 1.0e-6))
        return ((float) 0.0);
    xx = (float) exp( (float) (4.0 * xx / (xx - 0.203636)));
    return (xx);
}