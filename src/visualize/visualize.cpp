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

const int height = 24;
const int width = 32;
const int size = width * height;
int PIXEL_SIZE = 10;

vector<Point2f> getMaximumPoints(Mat data, int minValue, int maxCount, int minDistance);
/*
Mat matrixFromPointVector(vector<Point2f> vec) {
    int sz[1] = {vec.size()};
    Mat mat(1, sz, CV_32FC2);
    cout << "Created mat with dims: " << mat.dims << endl;
    int i = 0;
    for(Point2f p : vec) {
        mat.at<Point2f>(i++,0) = p;
    }
    return mat;
}
*/

Vec3b colorFromData(float it, int min, int max) {
    if(it < 0) 
    {
        it /= float(min);
        if(it < 0.5)
            return Vec3b(512 * it, 0, 0);
        else if (it > 1)   
            return Vec3b(255,255,255);
        else
            return Vec3b(255, 512*(it -0.5), 512*(it - 0.5));
    }
    else
    {
        it /= float(max);
        if(it < 0.5)
            return Vec3b(0, 512 * it, 0);
        else if (it > 1)
            return Vec3b(255,255,255);
        else
            return Vec3b(512 * (it - 0.5), 255, 512*(it - 0.5));
    }
}

int main (int argc, char **argv) {
    Mat horseImage = imread("horse1.png", CV_LOAD_IMAGE_COLOR);

    Mat smallData = Mat::zeros(height, width, CV_16SC1);
    Mat bigData = Mat::zeros(height * PIXEL_SIZE, width * PIXEL_SIZE, CV_16SC1);
	Mat bigImage = Mat::zeros(height * PIXEL_SIZE, width * PIXEL_SIZE, CV_8UC3);

    ifstream fs("pipe");
    string line, cell;
    getline(fs,line); // skip first line

    vector<Point2f> horseTemplate;
    //horseTemplate.push_back(Point2f(13,16));
    //horseTemplate.push_back(Point2f(25,15)); 
    //horseTemplate.push_back(Point2f( 5,12));
    //horseTemplate.push_back(Point2f(25,10));
    horseTemplate.push_back(Point2f(280,310));
    horseTemplate.push_back(Point2f(600,305)); 
    horseTemplate.push_back(Point2f(640,1320));
    horseTemplate.push_back(Point2f(300,2000));

    while(getline(fs,line)) {
        stringstream lineStream(line);
        getline(lineStream, cell, ','); // skip first column
        getline(lineStream, cell, ','); // skip second column

        // read data from stream into matrix
        for(int y = 0; y < height; y++) {
            for(int x = width - 1; x >= 0; x--) {
                if(getline(lineStream,cell, ',')) {
                    smallData.at<short>(Point(x, y)) = stoi(cell);
                } else {
                    smallData.at<short>(Point(x, y)) = 0;
                };
            }
        }

        resize(smallData, bigData, Size(width * PIXEL_SIZE, height * PIXEL_SIZE), 0, 0, INTER_CUBIC);

        // get min and max values
        int min = 0;
        int max = 0;
        for(int x = 0; x < smallData.cols; x++)  {
            for(int y = 0; y < smallData.rows; y++) { 
                int v =  smallData.at<short>(y,x);
                if(v < min) min = v; 
                if(v > max) max = v;
            }
        }
        min *= 1.1;
        max *= 1.1;
        int absmax = abs(min) > abs(max) ? abs(min) : abs(max);
        cout << "Min: "<< min << ", Max: "<< max << endl;

        // find maximum points
        vector<Point2f> points = getMaximumPoints(bigData, 350, 10, 25);

        // create visible image
        //cout << "bigData: " << bigData.cols << " cols * " << bigData.rows << " rows, type = " << bigData.type() << endl;
        //cout << "bigImage: " << bigImage.cols << " cols * " << bigImage.rows << " rows, type = " << bigImage.type() << endl;
        for(int x = 0; x < bigData.cols; x++)  {
            //cout << "x:" << x << ", ";
            for(int y = 0; y < bigData.rows; y++) { 
                //cout << "(y:" << y << ",x:" << x<< ") ";
                Vec3b color = colorFromData(bigData.at<short>(y,x), min, max);
                bigImage.at<Vec3b>(y,x) = color;
            }
        }


        for(Point2f p : points) {
            circle(bigImage, p, 16, CvScalar(0,0,255,255), 1);
        }

        if(points.size() >= 13) {
            /*
            Mat A = matrixFromPointVector(points);
            Mat B = matrixFromPointVector(horseTemplate);
            cout << "Matrices: " << A << " , " << B << endl;
            cout << "Dims: " << A.dims << " , " << B.dims << endl;
            Mat hom = estimateRigidTransform(A, B, false);
            */

            //Mat hom = estimateRigidTransform(points, horseTemplate, false);
/*
            vector<Point2f> pointsA, pointsB;
            for(Point2f a : horseTemplate)
                for(Point2f b : points) {
                    pointsA.push_back(a);
                    pointsB.push_back(b);
                }

            Mat hom = findHomography(pointsA, pointsB, CV_RANSAC, 130);
            if(hom.total() > 0) {
                float scaleX = norm(hom.col(0));
                float scaleY = norm(hom.col(1));
                cout << scaleX << " x " << scaleY << endl;


                Mat transformedHorseImage;
                warpPerspective(horseImage, transformedHorseImage, hom, Size(image.cols, image.rows));
                imshow("Horse", transformedHorseImage);
            } else {
                cout << "No homography found." << endl;
            }
            */
        }


        
        imshow("Raw Data", bigImage);
        waitKey(5);
    }
    waitKey(0);
}


vector<Point2f> getMaximumPoints(Mat data, int minValue, int maxCount, int minDistance)
{
    vector<Point2f> points;
    Point maxPoint;
    for(int i = 0; i < maxCount; i++) {
        int max = 0;    
        for(int x = 0; x < data.cols; x++) {
            for(int y = 0; y < data.rows; y++) {
                int v =  data.at<short>(y,x);
                if(v > max) {
                    Point2f newPoint(x,y);
                    for(Point2f p : points)
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