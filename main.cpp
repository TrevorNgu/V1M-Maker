#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <math.h>
#include <map>
#include <set>
#include <opencv2/opencv.hpp>
#include <filesystem>

#include "constant.h"
#include "filter.cpp"

using namespace std;

int createMonochrome(int red, int green, int blue);
int createRedGreen(int blue, int green, int red);
int createBlueYellow(int blue, int green, int red);
int setHeader(string mNum, int width, int height, int filterDiam, int vertSpace, int horSpace, int firstHorPos, int firstVertPos, ofstream &of);
void setFilters(ifstream &file, ifstream &filetxt, cv::Mat &image, int* headers, int* lastPoints);
float bump( float x);
void getDiff(ifstream &file, int* diff);

map<int, vector<Filter>> pair_filters;
map<int, vector<int>> col_row;
vector<int> col_vals;

int main(int argc, char *argv[]) {
    string command = argv[1];
    vector<string> files;

    // Checks argument prompt to see to target specific file or all 
    if(command == "ALL") {
        for(const auto & entry : filesystem::directory_iterator(to_convert)) {
            filesystem::path filepath = entry.path();
            string filename_str = filepath.string();
            filename_str.erase(filename_str.begin(), filename_str.begin() + 17);

            if(filename_str.substr(filename_str.size() - 4) != ".txt") {
                files.push_back(filename_str);
            }
        }

        set<string> s(files.begin(), files.end());
        files.assign(s.begin(), s.end());

        for(string n : files) {
            cout << n << endl;
        }
    } else {
        files.push_back(command);
    }


    int maxWidth = 0;
    int maxHeight = 0;

    // Processing all the files
    int current = 0;
    while(current != files.size()) {
        // Check if files exist, if so proceed
        command = files.at(current);
        ifstream file(to_convert + command);
        string ext = command.substr(command.size() - 3);
        command.erase(command.end() - 4, command.end());

        ifstream filetxt(to_convert + command + ".txt");
        
        // Continue if that's the case
        if(!file.good() || !filetxt.good()) {
            cerr << "Error! One of the files does not exist." << endl;
            current++;
            continue;
        }

        // Disgusting parsing of the PGM/PPM file
        string line;
        getline(file, line);
        cout << "Version: " << line << endl;
        getline(file, line);
        cout << "Size (W x H): " << line << endl;
        getline(file, line);
        cout << "Max gray value: " << line << endl;

        // Creating the V1M/V1R/V1B file format
        string convert;
        ofstream to_file;
        if(ext == "ppm") {
            convert = converted + command + ".v1r";
            to_file.open(convert);   
        } else {
            convert = converted + command + ".v1m";
            to_file.open(convert);
        }

        // More disgusting parsing of the first 2 lines from the QDED text file
        string linetxt;
        getline(filetxt, linetxt);
        getline(filetxt, linetxt);

        // Gets the image stored into the image var
        cv::Mat image;
        image = cv::imread(to_convert + command + "." + ext, 1);

        int headers[3];
        int lastPoints[2];
        setFilters(file, filetxt, image, headers, lastPoints);
        
        // If PPM, set header to V5 instead.
        if(ext == "ppm") {
            magic = "V5";
        }
        
        // Set the header
        setHeader(magic, image.rows, image.cols, headers[0], row_space, col_space, headers[1], headers[2], to_file);
        cout << lastPoints[0] << " " << lastPoints[1] << endl;

        // DEBUG: Check for the largest width and height
        if(lastPoints[0] + 1 > maxWidth) {
            maxWidth = lastPoints[0] + 1;
        }

        if(lastPoints[1] + 1 > maxHeight) {
            maxHeight = lastPoints[1] + 1;
        }

        // Processing writing into file
        for(int i = 0; i < lastPoints[1] + 1; i++) {
            for(int j = 0; j < lastPoints[0] + 1; j++) {
                int8_t first = pair_filters[i].at(j).firstFil + 128;
                int8_t second = pair_filters[i].at(j).secFil + 128;
                int8_t third = pair_filters[i].at(j).thirdFil + 128;
                int8_t fourth = pair_filters[i].at(j).fourthFil + 128;
                int8_t fifth = pair_filters[i].at(j).fifthFil + 128;

                to_file << first << second << third << fourth << fifth;

                if(ext == "ppm") {
                    int8_t rg = 0;
                    //pair_filters[i].at(j).RGChrom + 128;
                    int8_t by = 0;
                    //pair_filters[i].at(j).BYChrom + 128;
                    to_file << rg << by;
                } else {
                    int8_t rg = 127;
                    //pair_filters[i].at(j).RGChrom + 128;
                    int8_t by = 127;
                    //pair_filters[i].at(j).BYChrom + 128;
                    to_file << rg << by;
                }
            }
        }

        // DEBUG: Show the image.
        /*cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
        cv::imshow("Display Image", image);
        cv::waitKey(0);*/
        file.close();
        filetxt.close();
        to_file.close();
        current++;
    }

    // DEBUG: Just a way to see the largest width in all images and the largest height in all images
    //cout << "Max is: " << maxWidth << " " << maxHeight << endl;
}

// Takes BGR values and calculates the Monochrome value
int createMonochrome(int blue, int green, int red) {
    return (blue + green + red) / 3;
}

// Takes BGR values and calculates R-G Chrominance
int createRedGreen(int blue, int green, int red) {
    int rg = ((red - green) / 2) + 128;
    if(rg > 255) {
        return 255;
    } else if(rg < 0) {
        return 0;
    }

    return rg;
}

// Takes BGR values and calculates B-Y Chrominance
int createBlueYellow(int blue, int green, int red) {
    int by = ((2 * blue - red - green) / 4) + 128;
    if(by > 255) {
        return 255;
    } else if(by < 0) {
        return 0;
    }

    return by;
}

// Sets header into file
int setHeader(string mNum, int width, int height, int filterDiam, int vertSpace, int horSpace, int firstHorPos, int firstVertPos, ofstream &file) {
    file << mNum << "\n";
    file << width << "\n";
    file << height << "\n";
    file << diameter << "\n";
    file << filterDiam << "\n";
    file << vertSpace << "\n";
    file << horSpace << "\n";
    file << firstHorPos << "\n";
    file << firstVertPos << "\n";
    
    return 0;
}

// Sets filters into global map variable
void setFilters(ifstream &file, ifstream &filetxt, cv::Mat &image, int* headers, int* lastPoints) {
    bool first = true;
    vector<vector<Filter>> filters;

    // Get image and set to YCrCb
    cv::cvtColor(image, image, cv::COLOR_BGR2YCrCb);
    int pointx;
    int pointy;

    int diff[2]; 
    getDiff(filetxt, diff);
    int secondColVal = -1;
    int secondRowVal = -1;
    while(filetxt.good()) {
        string parse[9];
        filetxt >> parse[0] >> parse[1] >> parse[2] >> parse[3] >> parse[4] >> parse[5] >> parse[6] >> parse[7] >> parse[8];

        // Disgusting way to check whether we've hit the end of the file
        if(parse[0] == "") {
            break;
        }

        // Pulls all variables from text file
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

        // Bad way of confirming the first tile on the top left most area
        if(first == true) {
            headers[0] = dia;
            headers[1] = col;
            headers[2] = row;
            first = false;
        }

        // Set variables
        cv::Point p(col, row);
        int x = col;
        int y = row;
        int r = rad;
        float firstFilter = 0;
        float secondFilter = 0;
        float thirdFilter = 0;
        float fourthFilter = 0;
        float fifthFilter = 0;
        float rg = 0;
        float by = 0;

        pointx = col / diff[0];
        pointy = row / diff[1];

        // DEBUG: Uncomment to see circles
        //cv::circle(image, p, rad, (0, 0, 255), 1);
        //cout << pointx << ", " << pointy << endl;

        // Within the diameter of the 1st current filter, calculate for each of the 5 numbers
        for (int px = x - r; px <= x + r; px++) {
            float totalcy = 0;
            float totalcz = 0;
            for (int py = y - r; py <= y + r; py++) {
                int dx = x - px, dy = y - py;
                
                if (dx * dx + dy * dy <= r * r) {
                // Point is part of the circle.
                    cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(px, py));
                    float cx = color[0];
                    float cy = color[1];
                    float cz = color[2];
                    firstFilter += color[0] * filter[0];
                    secondFilter += color[0] * filter[1];
                    thirdFilter += color[0] * filter[2];
                    fourthFilter += color[0] * filter[3];
                    fifthFilter += color[0] * filter[4];
                    totalcy += cy;
                    totalcz += cz;

                    //cout << cx << " " << cy << " " << cz << endl;
                }
            }

            //cout << totalcy << " " << totalcz << endl;
        }
        
        // Divide ALL filters by the divide value defined in constant.h so it can fit into 1 byte
        firstFilter /= divide_val;
        secondFilter /= divide_val;
        thirdFilter /= divide_val;
        fourthFilter /= divide_val;
        fifthFilter /= divide_val;

        // Round to closest number
        round(firstFilter);
        round(secondFilter);
        round(thirdFilter);
        round(fourthFilter);
        round(fifthFilter);

        // Set filter
        Filter fil;
        fil.firstFil = firstFilter;
        fil.secFil = secondFilter;
        fil.thirdFil = thirdFilter;
        fil.fourthFil = fourthFilter;
        fil.fifthFil = fifthFilter;
        //fil.debug = to_string(pointx) + " " + to_string(pointy);
        //fil.debugpix = to_string(col) + " " + to_string(row);

        //cout << fil.firstFil << " " << fil.secFil << " " << fil.thirdFil << " " << fil.fourthFil << " " << fil.fifthFil << endl;

        if(true) {
            pair_filters[pointy].push_back(fil);
        }

    }
    cvtColor(image, image, cv::COLOR_YCrCb2BGR);

    lastPoints[0] = pointx;
    lastPoints[1] = pointy;
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

// Calculate the difference between the 1st and 2nd axis points (it just makes it easier to calculate where to start with each pixel on a file by file basis)
void getDiff(ifstream &filetxt, int* diff) {
    streampos before = filetxt.tellg();
    int firstCol = -1;
    int secondCol = -1;
    int firstRow = -1;
    int secondRow = -1;
    while(true) {
        string parse[9];
        filetxt >> parse[0] >> parse[1] >> parse[2] >> parse[3] >> parse[4] >> parse[5] >> parse[6] >> parse[7] >> parse[8];
        int col = stoi(parse[1]);
        int row = stoi(parse[2]);

        if(firstCol == -1) {
            firstCol = col;
        }

        if(firstRow == -1) {
            firstRow = row;
        }

        if(secondCol == -1 && firstCol != col) {
            secondCol = col;
            break;
        }

        if(secondRow == -1 && firstRow != row) {
            secondRow = row;
        }
    }

    diff[0] = secondCol - firstCol;
    diff[1] = secondRow - firstRow;

    cout << diff[0] << " " << diff[1] << endl;

    filetxt.seekg(before);
}