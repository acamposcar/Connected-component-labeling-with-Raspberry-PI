/*******************************************
   Connected-component labeling - One pass
********************************************/
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "/home/pi/raspicam-0.1.3/src/raspicam_cv.h"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;

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

    cout<<"Connecting to camera"<<endl;

    if ( !Camera.open() ) {
        cerr<<"Error opening camera"<<endl;
        return -1;
    }

    cout<<"Connected to camera ="<<Camera.getId() <<endl;


    // Initialize variables

    cv::Mat image; // Original image
    unsigned char gr;

    int threshold ;
    cout << "Select threshold  (0 - 255): ";
    cin >> threshold  ;

    cout<<"Processing..."<<endl;



    while (1) {

      Camera.grab();
      Camera.retrieve (image);

      cv::Mat label; // Postprocessed image
      label = cv::Mat::zeros(image.rows, image.cols, CV_8U); // 'label' and 'image' should have same size


      // Segmentation 

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
      }


      // "Label" image to white (255)

      for(int i=0; i<image.cols; i++){
			  for(int j=0; j<image.rows; j++){

	 			  label.at<unsigned char>(j,i)=255;
			  }
     	}

     	int etNum=0; // Label number
     	int found=true;
     	int finished=false;

     	// Search for pixels of interest

      while(found==true){

       	found=false;

       	for(int i=0; i<image.cols; i++){
				  for(int j=0; j<image.rows; j++){

	  				if(image.at<unsigned char>(j,i)==0 && label.at<unsigned char>(j,i)==255 && found==false){
	    				found=true;
	    				etNum++;
	    				label.at<unsigned char>(j,i)=etNum;
	  				}

				  }
       	}

	   		if(found){
	      		finished=false;
	     	}

	    	
	    	// Search for neighbours

	    	while(finished == false){

	      	finished = true;

	      	for(int i=1; i<image.cols-1; i++){

					   for(int j=1; j<image.rows-1; j++){

		  				if(image.at<unsigned char>(j,i)==0 && label.at<unsigned char>(j,i)==255 && ( label.at<unsigned char>(j,i+1)==etNum ||  label.at<unsigned char>(j-1,i+1)==etNum ||  label.at<unsigned char>(j-1,i)==etNum ||  label.at<unsigned char>(j-1,i-1)==etNum ||  label.at<unsigned char>(j,i-1)==etNum ||  label.at<unsigned char>(j+1,i-1)==etNum ||  label.at<unsigned char>(j+1,i)==etNum ||  label.at<unsigned char>(j+1,i+1)==etNum)){
		    				
		    				finished = false;
		    				label.at<unsigned char>(j,i) = etNum;
		  				}

					   }
	      	}
		    } 

		    
      }
     

		cout<<"Number of objects ="<<etNum <<endl;

    cv::imshow ("Binarized",image);
    cv::imshow("Labeled",label);

    if(cv::waitKey(30)==27){
	 	   break;
		}

    cv::waitKey(100);

  }

  Camera.release();

}
