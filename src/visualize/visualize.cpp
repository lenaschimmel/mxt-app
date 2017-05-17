#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace cv;
using namespace std;

const int height = 32;
const int width = 24;
const int size = width * height;
int PIXEL_SIZE = 10;

vector<Point> getMaximumPoints(Mat data, int minValue, int maxCount, int minDistance);

int main (int argc, char **argv) {
    Mat data = Mat::zeros(height, width, CV_16SC1);
	Mat image = Mat::zeros(height * PIXEL_SIZE, width * PIXEL_SIZE, CV_8UC3);

    ifstream fs("pipe");
    string line, cell;
    getline(fs,line); // skip first line

    while(getline(fs,line)) {
        stringstream lineStream(line);
        getline(lineStream, cell, ','); // skip first column
        getline(lineStream, cell, ','); // skip second column

        // read data from stream into matrix
        for(int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                if(getline(lineStream,cell, ',')) {
                    data.at<short>(x,y) = stoi(cell);
                } else {
                    data.at<short>(x, y) = 0;
                };
            }
        }

        // get min and max values
        int min = 0;
        int max = 0;
        for(int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                int v =  data.at<short>(x,y);
                if(v < min) min = v; 
                if(v > max) max = v;
            }
        }
        int absmax = abs(min) > abs(max) ? abs(min) : abs(max);

        // find maximum points
        vector<Point> points = getMaximumPoints(data, 100, 10, 4);

        // create visible image
        CvScalar color;
        for(int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                float it = data.at<short>(x, y);
                if(it < 0) 
                {
                    it /= float(min);
                    if(it < 0.5)
                        color = CvScalar(512 * it, 0, 0, 255);
                    else
                        color = CvScalar(255, 512*(it -0.5), 512*(it - 0.5), 255);
                }
                else
                {
                    it /= float(max);
                    if(it < 0.5)
                        color = CvScalar(0, 512 * it, 0, 255);
                    else
                        color = CvScalar(512 * (it - 0.5), 255, 512*(it - 0.5), 255);
                }

                rectangle(image, CvPoint(x * PIXEL_SIZE, y * PIXEL_SIZE), CvPoint( (x+1) * PIXEL_SIZE, (y+1) * PIXEL_SIZE), color, CV_FILLED);
            }
        }

        Point halfPixel(PIXEL_SIZE / 2, PIXEL_SIZE / 2);
        cout << endl;
        for(Point p : points) {
            circle(image, p * PIXEL_SIZE + halfPixel, 16, CvScalar(0,0,255,255), 3);
            cout << data.at<short>(p.x, p.y) << " ";
        }
        
        imshow("Raw Data", image);
        waitKey(5);
    }
    waitKey(0);
}

vector<Point> getMaximumPoints(Mat data, int minValue, int maxCount, int minDistance)
{
    vector<Point> points;
    Point maxPoint;
    for(int i = 0; i < maxCount; i++) {
        int max = 0;    
        for(int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                int v =  data.at<short>(x,y);
                if(v > max) {
                    Point newPoint(x,y);
                    for(Point p : points)
                        if(norm(p - newPoint) < minDistance)
                            goto nextPoint;
                    max = v;
                    maxPoint = newPoint;
                }
                nextPoint: ;
            }
        }
        if(max > minValue) {
            points.push_back(maxPoint);
        } else {
            break;
        }
    }
    return points;
}