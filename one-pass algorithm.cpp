/*******************************************
Connected component labeling with Raspberry Pi using OpenCV and C++

Authors:  Alejandro Campos Carceller [acamposcar@gmail.com]
          José Tomás Mora

The algorithm has been tested in a Raspberry Pi 2 using the Camera Module. 
https://www.raspberrypi.org/products/camera-module/

More info about one-pass and two-pass algorithm at 
https://en.wikipedia.org/wiki/Connected-component_labeling
********************************************/
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include "/home/pi/raspicam-0.1.3/src/raspicam_cv.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main ( int argc,char **argv ) {

  // Pi camera initialization
    
  raspicam::RaspiCam_Cv Camera;

  Camera.set ( CV_CAP_PROP_FRAME_WIDTH, 320 );
  Camera.set ( CV_CAP_PROP_FRAME_HEIGHT, 240 );
  Camera.set ( CV_CAP_PROP_BRIGHTNESS, 50 );
  Camera.set ( CV_CAP_PROP_CONTRAST, 50 );
  Camera.set ( CV_CAP_PROP_SATURATION, 50 );
  Camera.set ( CV_CAP_PROP_GAIN, 50 );
  Camera.set ( CV_CAP_PROP_FORMAT, CV_8UC1 ); //GRAY
  //Camera.set ( CV_CAP_PROP_FORMAT,CV_8UC3); //BGR
     

  cout<<"Connecting to camera"<<endl;

  if ( !Camera.open() ) {
    cerr<<"Error opening camera"<<endl;
    return -1;
  }

  cout<<"Connected to camera ="<<Camera.getId() <<endl;

  // Initialize variables

  cv::Mat image; // Original image
  
  unsigned char gr;
  int treshold;
  int erosion;

  cout<<"Select threshold  (0 - 255): ";
  cin>>treshold;
  
  cout<<"Erosion size: ";
  cin>>erosion;

  cout<<"Processing..."<<endl;


    while (1) {
      Camera.grab();
      Camera.retrieve (image);

      cv::Mat label; // Label image
      label = cv::Mat::zeros(image.rows, image.cols, CV_8U);

      cv::Mat color; // Color image
      color = cv::Mat::zeros(image.rows-1, image.cols-1, CV_8UC3);

      cv::Mat ero; // Erosion image
      ero= cv::Mat::zeros(image.rows, image.cols, CV_8U);

      cv::Mat element=getStructuringElement(2, cv::Size(2*erosion+1,2*erosion+1),cv::Point(erosion,erosion));
      dilate(image,ero,element);  
      

      cv::imshow("Original",image);

      // Segmentation: create binary image from source image 

      for(int i=0; i<image.cols; i++){
        for(int j=0; j<image.rows; j++){

          gr=image.at<unsigned char>(j,i);

          if(gr > threshold){
            image.at<unsigned char>(j,i)=255;
          }

          else {
            image.at<unsigned char>(j,i)=0;
          }

        }

      // "Label" image to white (255)

   
      for(int i=0; i<image.cols; i++){
        for(int j=0; j<image.rows; j++){

          label.at<unsigned char>(j,i)=255;
        }
      }

     int labNum=0; // Label number
     int found=true;
     int finished=false;
     int colNum=0; // Color number

     // Search for pixels of interest

      while(found==true){

        found=false;

        for(int i=1; i<image.cols-1; i++){
          for(int j=1; j<image.rows-1; j++){

            if(image.at<unsigned char>(j,i)==0 && label.at<unsigned char>(j,i)==255 && found==false){
              found=true;
              labNum++;
              label.at<unsigned char>(j,i)=labNum;
            }

          }
        }

        if(found){
            finished=false;
        }
	    
      // Search for neighbours

	      while(finished==false){

          finished=true;

          for(int i=1; i<image.cols-1; i++){
		        for(int j=1; j<image.rows-1; j++){

              if(ero.at<unsigned char>(j,i)==0 && label.at<unsigned char>(j,i)==255 && ( label.at<unsigned char>(j,i+1)==labNum ||  label.at<unsigned char>(j-1,i+1)==labNum ||  label.at<unsigned char>(j-1,i)==labNum ||  label.at<unsigned char>(j-1,i-1)==labNum ||  label.at<unsigned char>(j,i-1)==labNum ||  label.at<unsigned char>(j+1,i-1)==labNum ||  label.at<unsigned char>(j+1,i)==labNum ||  label.at<unsigned char>(j+1,i+1)==labNum)){
		          finished=false;
		          label.at<unsigned char>(j,i)=labNum;
		          }
		        }
	       }
		  } 
    }

    // Create color image

	  for(int i=1;i<image.cols-1;i++){
	    for(int j=1;j<image.rows-1;j++){
	      if(label.at<unsigned char>(j,i)!=255 && label.at<unsigned char>(j,i)!=0){

		      colNum=label.at<unsigned char>(j,i);
		      color.at<cv::Vec3b>(j,i)[0]=55 + colNum * 10 *((-1)^colNum);
		      color.at<cv::Vec3b>(j,i)[1]=128 + ((-1)^colNum) * 10 *colNum;
		      color.at<cv::Vec3b>(j,i)[2]=255 - colNum*15;
		    }
	    }
	  }
     


	  cout<<"Total objects ="<<labNum <<endl;
	   
    cv::imshow("Erosion",ero);
    cv::imshow ("Binary",image);
    cv::imshow("Color",color);

    if(cv::waitKey(30)==27){  // Press Esc to exit
	    break; 
	  }

    cv::waitKey(100);

  }

    Camera.release();

}
