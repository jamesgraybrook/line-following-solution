// Include files for required libraries
#include <stdio.h>

#include "opencv_aee.hpp"
#include "main.hpp"     // You can use this file for declaring defined values and functions
#include "pi2c.h"



#include "ColourLineDetector.hpp"


Pi2c car(0x22); // Configure the I2C interface to the Car as a global variable
Pi2c arduino(7);
void setup(){
    setupCamera(320, 240);  // Enable the camera for OpenCV
}

int main( int argc, char** argv )
{
    setup();    // Call a setup function to prepare IO and devices
    Pi2c arduino(7);   // Create a GUI window called photo
    cv::namedWindow("Output frame",WINDOW_AUTOSIZE);
    cv::namedWindow("Output2",WINDOW_AUTOSIZE);
    cv::namedWindow("Output",WINDOW_AUTOSIZE);
    //global variables
    float perror;
    int correction;
    int symbol;
    float sum;
    int clock = 20;
    Mat CIRCLE = imread("Circle (Red Line).png");
    cvtColor(CIRCLE, CIRCLE, COLOR_BGR2HSV);
    inRange(CIRCLE, Scalar(150,30,30), Scalar(180,220,220), CIRCLE);

    Mat STAR = imread("Star (Green Line).png");
    cvtColor(STAR, STAR, COLOR_BGR2HSV);
    inRange(STAR, Scalar(150,30,30), Scalar(180,220,220), STAR);

    Mat TRIANGLE = imread("Triangle (Blue Line).png");
    cvtColor(TRIANGLE, TRIANGLE, COLOR_BGR2HSV);
    inRange(TRIANGLE, Scalar(150,30,30), Scalar(180,220,220), TRIANGLE);

    Mat UMBRELLA = imread("Umbrella (Yellow Line).png");
    cvtColor(UMBRELLA, UMBRELLA, COLOR_BGR2HSV);
    inRange(UMBRELLA, Scalar(150,30,30), Scalar(180,220,220), UMBRELLA);

    while(1)    // Main loop to perform image processing
    {
        Mat frame;
        while(frame.empty()){
            frame = captureFrame();
        }// Capture a frame from the camera and store in a new matrix variable
        cv::flip(frame,frame,-1);

        Mat image;
        image=frame(Rect(0,100,280,100));  //chop off top half on camera
        int wavg;

        // MAKE SYMBOL RECOGNITION HAPPEN EVERY FEW SECONDS
        if(clock == 20){
            symbol = SymbolRecognition(frame,CIRCLE,STAR,UMBRELLA,TRIANGLE);
            clock = 0;
        }


        if (symbol == 0){
        wavg = FindLine(image, correction); //Find line and calculate wavg
        if(450 < wavg){
        correction = 499;
        }
        if(-450 > wavg){
        correction = -499;
        }
        float fwAvg = ((float)wavg);
        float error = (0.1 * fwAvg);
        if(error == 0 && (perror > 38 || perror < -38)){
        error = 100;
        }
        sum = sum + error;
        float PID = DoPID(error, sum, perror);
        perror = error;
        cout<<"blacl";
        //cout << "The PID  is " << PID <<endl;
        arduino.i2cWriteArduinoInt(PID);  //send PID to arduino
        }



        if (symbol == 1){
        wavg = RedLine(image, correction); //Find line and calculate wavg
        if(450 < wavg){
        correction = 499;
        }
        if(-450 > wavg){
        correction = -499;
        }
        float fwAvg = ((float)wavg);
        float error = (0.1 * fwAvg);
        if(error == 0 && (perror > 38 || perror < -38)){
        error = perror;
        }
        sum = sum + error;
        float PID = DoPID(error, sum, perror);
        perror = error;
        cout<<"red";
        //cout << "The PID  is " << PID <<endl;
        arduino.i2cWriteArduinoInt(PID);  //send PID to arduino
        }



        if (symbol == 2){
        wavg = GreenLine(image, correction); //Find line and calculate wavg
        if(450 < wavg){
        correction = 499;
        }
        if(-450 > wavg){
        correction = -499;
        }
        float fwAvg = ((float)wavg);
        float error = (0.1 * fwAvg);
        if(error == 0 && (perror > 38 || perror < -38)){
        error = perror;
        }
        sum = sum + error;
        float PID = DoPID(error, sum, perror);
        perror = error;
        cout<<"green";
        //cout << "The PID  is " << PID <<endl;
        arduino.i2cWriteArduinoInt(PID);  //send PID to arduino
        }



        if (symbol == 3){
        wavg = BlueLine(image, correction); //Find line and calculate wavg
        if(450 < wavg){
        correction = 499;
        }
        if(-450 > wavg){
        correction = -499;
        }
        float fwAvg = ((float)wavg);
        float error = (0.1 * fwAvg);
        if(error == 0 && (perror > 38 || perror < -38)){
        error = perror;
        }
        sum = sum + error;
        float PID = DoPID(error, sum, perror);
        perror = error;
        cout<<"blue";
        //cout << "The PID  is " << PID <<endl;
        arduino.i2cWriteArduinoInt(PID);  //send PID to arduino
        }



        if (symbol == 4){
        wavg = YellowLine(image, correction); //Find line and calculate wavg
        if(450 < wavg){
        correction = 499;
        }
        if(-450 > wavg){
        correction = -499;
        }
        float fwAvg = ((float)wavg);
        float error = (0.1 * fwAvg);
        if(error == 0 && (perror > 38 || perror < -38)){
        error = perror;
        }
        sum = sum + error;
        float PID = DoPID(error, sum, perror);
        perror = error;
        cout<<"yellow";
        //cout << "The PID  is " << PID <<endl;
        arduino.i2cWriteArduinoInt(PID);  //send PID to arduino
        }


        clock = clock + 1;

        //end of loop
        int key = cv::waitKey(1);   // Wait 1ms for a keypress (required to update windows)

        key = (key==255) ? -1 : key;    // Check if the ESC key has been pressed
        if (key == 27)
            break;
	}

	closeCV();  // Disable the camera and close any windows

	return 0;
}