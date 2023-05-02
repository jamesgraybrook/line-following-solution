// Include files for required libraries

#include "opencv_aee.hpp"
#include "opencv2/imgcodecs.hpp"
#include <random>
#include <iostream>
#include <fstream>


using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;


float DoPID(float error, float sum, float perror) {
    float result,kp,ki,kd;
    kp = 0.54;
    ki = 0.00001;
    kd = 0.33;
    result = kp * error + ki * sum + kd * (error - perror);
    return result;
}





int SymbolRecognition(Mat frame,Mat CIRCLE,Mat STAR,Mat UMBRELLA,Mat TRIANGLE){

    Mat framecopy = frame.clone(); // create a copy of the input frame
    Mat image;
    cvtColor(frame, image, COLOR_BGR2HSV);  //convert image
    GaussianBlur(image, image, Size(7, 7), 0);

    // Create a binary mask with the selected pink color range
    Mat pink_mask;
    inRange(image, Scalar(150,30,30), Scalar(180,240,240), pink_mask);
    int pixels;
    pixels = countNonZero(pink_mask);
            imshow("Output frame", pink_mask);
    if(pixels < 5000){
        int result = 0;
        return(result);
    }
    else{
    // Find the contour with the biggest area in the pink mask
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    int savedContour = -1;
    double maxArea = 0.0;
    findContours(pink_mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

    for (int i = 0; i< contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > maxArea)
        {
            maxArea = area;
            savedContour = i;
        }
    }

    // Create a mask from the largest pink contour and draw it onto a copy of the input frame
    Mat mask = Mat::zeros(frame.size(), CV_8UC1);
    drawContours(mask, contours, savedContour, Scalar(255), FILLED, 8);
    Mat masked_image;
    frame.copyTo(masked_image, mask);
    //imshow("output_frame",masked_image);

    // Find the contours of the pink mask to extract the outermost contour
vector<vector<Point>> contours2;
vector<Vec4i> hierarchy2;
findContours(mask, contours2, hierarchy2, RETR_TREE, CHAIN_APPROX_SIMPLE);
// Find the four corners of the outermost contour using approxPolyDP
vector<Point> contour_poly;
approxPolyDP(contours2[0], contour_poly, 3, true);
vector<Point2i> corners;
for (int i = 0; i < contour_poly.size(); i++) {
    corners.push_back(Point2i(contour_poly[i].x, contour_poly[i].y));
}
// Sort the corners in clockwise order, starting from the top left corner
sort(corners.begin(), corners.end(), [](const Point2i& a, const Point2i& b) {
    return a.y < b.y;
});
vector<Point2i> corners_top = {corners[0], corners[1]};
vector<Point2i> corners_bottom = {corners[2], corners[3]};
sort(corners_top.begin(), corners_top.end(), [](const Point2i& a, const Point2i& b) {
    return a.x < b.x;
});
sort(corners_bottom.begin(), corners_bottom.end(), [](const Point2i& a, const Point2i& b) {
    return a.x > b.x;
});
corners = {corners_top[0], corners_top[1], corners_bottom[0], corners_bottom[1]};
// Define the destination points for the transformation
int xsize = 350;
int ysize = 350;
Point2f dst[4] = {Point2i(0, 0), Point2i(xsize, 0), Point2i(xsize, ysize), Point2i(0, ysize)};

// Perform the perspective transformation
Mat TRANSFORMED;
TRANSFORMED = transformPerspective(corners, pink_mask, xsize, ysize);
    //compare images

    float pC, pS, pT, pU; //create variables for percentage similarity of all symbols
    pC =  compareImages(TRANSFORMED, CIRCLE);    // Return % similarity
    pS =  compareImages(TRANSFORMED, STAR);
    pT =  compareImages(TRANSFORMED, TRIANGLE);
    pU =  compareImages(TRANSFORMED, UMBRELLA);



    if(pC > pS && pC > pT && pC > pU){
        int result = 1;
        imshow("Output frame", pink_mask);
        cout<<"c";
        return(result);
    }

    if(pS > pC && pS > pT && pS > pU){
        int result = 2;
        imshow("Output frame", pink_mask);
        cout<<"s";
        return(result);
    }
    if(pT > pC && pT > pS && pT > pU){
        int result = 3;
        imshow("Output frame", pink_mask);
        cout<<"t";
        return(result);
    }
    if(pU > pC && pU > pS && pU > pT){
        int result = 4;
        imshow("Output frame", pink_mask);
        cout<<"u";
        return(result);
    }

    else{
        int result = 0;
        return(result);
    }

}
}




int WeightAvg(Mat Output) {
        int height = 100;
        Mat image1 = Output(Range(0,height),Range(0,40));
        Mat image2 = Output(Range(0,height),Range(40,65));
        Mat image3 = Output(Range(0,height),Range(65,90));
        Mat image4 = Output(Range(0,height),Range(90,115));
        Mat image5 = Output(Range(0,height),Range(115,140));
        Mat image6 = Output(Range(0,height),Range(140,165));
        Mat image7 = Output(Range(0,height),Range(165,190));
        Mat image8 = Output(Range(0,height),Range(190,215));
        Mat image9 = Output(Range(0,height),Range(215,240));
        Mat image10 = Output(Range(0,height),Range(240,280));


    int x1, x2, x3, x4, x5,x6,x7,x8,x9,x10;
    x1 = countNonZero(image1);
    x2 = countNonZero(image2);
    x3 = countNonZero(image3);
    x4 = countNonZero(image4);
    x5 = countNonZero(image5);
    x6 = countNonZero(image6);
    x7 = countNonZero(image7);
    x8 = countNonZero(image8);
    x9 = countNonZero(image9);
    x10 = countNonZero(image10);
    int WAvg;
    WAvg = ((x1*-11)+(x2*-7)+(x3*-3)+(x4*-2)+(x5*-1)+(x6*1)+(x7*2)+(x8*3)+(x9*7)+(x10*11))*100;
    WAvg = WAvg/(x1+x2+x3+x4+x5+x6+x7+x8+x9+x10+1);
    return(WAvg);
}





int RedLine(Mat image, int correction) {
    cvtColor(image, image, COLOR_BGR2HSV);

    //GET RED
    Mat OutputRed;
    inRange(image, Scalar(160,35,40), Scalar(185,245,245), OutputRed);

    int x1;
    x1 = countNonZero(OutputRed);

    if(x1 > 0){
        //cout << "RED";
        imshow("Output",OutputRed);
        int WAvg;
        WAvg=WeightAvg(OutputRed);
        return(WAvg);
    }
    else{
        //cout << "Error";
        int WAvg;
        WAvg = correction;
        return(WAvg);
    }
}





int GreenLine(Mat image, int correction) {
    cvtColor(image, image, COLOR_BGR2HSV);

    //GET GREEN
    Mat OutputGreen;
    inRange(image, Scalar(75,55,40), Scalar(95,255,255), OutputGreen);

    //GET MAIN COLOUR
    int x2;
    x2 = countNonZero(OutputGreen);

    if (x2 > 0){
        //cout << "GREEN";
        imshow("Output",OutputGreen);
        int WAvg;
        WAvg=WeightAvg(OutputGreen);
        return(WAvg);
}
    else{
        //cout << "Error";
        int WAvg;
        WAvg = correction;
        return(WAvg);
    }
}





int BlueLine(Mat image, int correction) {
    cvtColor(image, image, COLOR_BGR2HSV);

    //GET BLUE
    Mat OutputBlue;
    inRange(image, Scalar(97,40,23), Scalar(120,255,250), OutputBlue);

    //GET MAIN COLOUR
    int x3;
    x3 = countNonZero(OutputBlue);

    if (x3 > 0){
        //cout << "BLUE";
        imshow("Output",OutputBlue);
        int WAvg;
        WAvg=WeightAvg(OutputBlue);
        return(WAvg);
}
    else{
        //cout << "Error";
        int WAvg;
        WAvg = correction;
        return(WAvg);
    }
}





int YellowLine(Mat image, int correction) {
    cvtColor(image, image, COLOR_BGR2HSV);

    //GET YELLOW
    Mat OutputYellow;
    inRange(image, Scalar(20,100,50), Scalar(40,255,255), OutputYellow);

    //GET MAIN COLOUR
    int x4;
    x4 = countNonZero(OutputYellow);

    if (x4 > 0){
        //cout << "YELLOW";
        imshow("Output",OutputYellow);
        int WAvg;
        WAvg=WeightAvg(OutputYellow);
        return(WAvg);
}
    else{
        //cout << "Error";
        int WAvg;
        WAvg = correction;
        return(WAvg);
    }
}





int FindLine(Mat image, int correction) {
    cvtColor(image, image, COLOR_BGR2HSV);

    //GET RED
    Mat OutputRed;
    inRange(image, Scalar(160,35,40), Scalar(185,245,245), OutputRed);

    //GET GREEN
    Mat OutputGreen;
    inRange(image, Scalar(75,55,40), Scalar(95,255,255), OutputGreen);

    //GET BLUE
    Mat OutputBlue;
    inRange(image, Scalar(97,40,23), Scalar(120,255,250), OutputBlue);

    //GET YELLOW
    Mat OutputYellow;
    inRange(image, Scalar(20,100,50), Scalar(40,255,255), OutputYellow);

    //GET BLACK
    Mat OutputBlack;
    inRange(image, Scalar(0,0,0), Scalar(360,100,90), OutputBlack);



    //GET MAIN COLOUR
    int x1, x2, x3, x4, x5;
    x1 = countNonZero(OutputRed);
    x2 = countNonZero(OutputGreen);
    x3 = countNonZero(OutputBlue);
    x4 = countNonZero(OutputYellow);
    x5 = countNonZero(OutputBlack);


    if(x1>x2 && x1>x3 && x1>x4 && x1>x5){
        //cout << "RED";
        //imshow("Output",OutputRed);
        int WAvg;
        WAvg=WeightAvg(OutputRed);
        return(WAvg);
    }
    else if (x2>x1 && x2>x3 && x2>x4 && x2>x5){
        //cout << "GREEN";
        //imshow("Output",OutputGreen);
        int WAvg;
        WAvg=WeightAvg(OutputGreen);
        return(WAvg);
}
    else if (x3>x1 && x3>x2 && x3>x4 && x3>x5){
        //cout << "BLUE";
        //imshow("Output",OutputBlue);
        int WAvg;
        WAvg=WeightAvg(OutputBlue);
        return(WAvg);
}
    else if (x4>x1 && x4>x2 && x4>x3 && x4>x5){
        //cout << "YELLOW";
        //imshow("Output",OutputYellow);
        int WAvg;
        WAvg=WeightAvg(OutputYellow);
        return(WAvg);
}
    else if (x5>x1 && x5>x2 && x5>x3 && x5>x4){
        //cout << "BLACK";
        //imshow("Output",OutputBlack);
        int WAvg;
        WAvg=WeightAvg(OutputBlack);
        return(WAvg);
}
    else{
        //cout << "Error";
        int WAvg;
        WAvg = correction;
        return(WAvg);
    }

}