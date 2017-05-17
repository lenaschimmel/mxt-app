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

int main (int argc, char **argv) {
    Mat image = Mat::zeros(height * PIXEL_SIZE, width * PIXEL_SIZE, CV_8UC3);
	
    int data_buf[size];


    ifstream fs("pipe");
    string line;
    getline(fs,line); // skip first line
    

    
    string                cell;

    while(getline(fs,line)) {
        cout << "Read a line..." << endl;
        stringstream lineStream(line);
        getline(lineStream,cell, ','); // skip first column
        getline(lineStream,cell, ','); // skip second column
        int i = 0;
        while(getline(lineStream,cell, ','))
        {
            //cout << "V:'" << cell << "'" << endl;
            data_buf[i++] = stoi(cell);
        }

        int min = 0;
        int max = 0;
        for (unsigned long i = 0; i < size; i++)
        {
            if(data_buf[i] < min) min = data_buf[i]; 
            if(data_buf[i] > max) max = data_buf[i]; 
        }
        cout << "Min value: " << min;
        cout << "Max value: " << max;
        int absmax = abs(min) > abs(max) ? abs(min) : abs(max);

        CvScalar color;
        for (unsigned long i = 0; i < size; i++)
        {
            int x = i / height;
            int y = i % height;

            float it = float(data_buf[i]);
            if(it < 0) 
            {
                it /= float(min);
                if(it < 0.5)
                    color = CvScalar(512 * it,0,0,255);
                else
                    color = CvScalar(255,512*(it -0.5),512*(it -0.5),255);
            }
            else
            {
                it /= float(max);
                if(it < 0.5)
                    color = CvScalar(0,512 * it,0,255);
                else
                    color = CvScalar(512*(it -0.5),255,512*(it -0.5),255);
            }

            rectangle(image, CvPoint(x * PIXEL_SIZE, y * PIXEL_SIZE), CvPoint( (x+1) * PIXEL_SIZE, (y+1) * PIXEL_SIZE), color, CV_FILLED);
        }
        
        imshow("Raw Data", image);
        waitKey(5);
    }
    waitKey(0);
}