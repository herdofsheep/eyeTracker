#include "inputManager.h"
#include "ofxPS3EyeGrabber.h"

inputManager::inputManager() {
}

void inputManager::setup(){

    //--- camera stuff
    //IF USING WEBCAM:
    //width = 320;
    //height = 240;
    //cam.setVideoID(0);    //replace cam.setGrabber(...);

    //FOR PS3EYE:
    width = 640;
    height = 480;
    cam.setGrabber(std::make_shared<ofxPS3EyeGrabber>());

    cam.setDesiredFrameRate(30);
    cam.initGrabber(width, height);

    grayImage = new ofxCvGrayscaleImage;

  //images setup
	if (width != 0 || height != 0){
        grayImage->allocate(width, height);
        colorImg.allocate(width, height);
	}

  //initialisation
	flipX = false;
	flipY = false;
	fcount = 0;
	bIsFrameNew = false;
}

void inputManager::update( ofxCvGrayscaleImage currentImage ){

    //a few images for dark/bright image finding processes
    cam.update();
    colorImg.setFromPixels(cam.getPixels());
    grayImg = colorImg;
    grayImage = &grayImg;
    grayOddImage = currentImage;
    grayEvenImage = currentImage;


  //update as new frames get pushed to grayImg.
	if (cam.isFrameNew()){

		fcount++;
		fcount = fcount % 2;
        bIsFrameNew = true;

		// it's important to flip the image *after* the embedded information is read
		// otherwise the embedded information will be moved before we can read it.
		if( flipX || flipY ){
			grayImage->mirror(flipY, flipX);
		}

		if (fcount == 0) grayEvenImage = *grayImage;
		else grayOddImage = *grayImage;
	}
}

//functions for clarity elsewhere.
void inputManager::drawOddFrame(int x, int y, int width, int height){
	grayOddImage.draw(x,y,width, height);
}

void inputManager::drawEvenFrame(int x, int y, int width, int height){
	grayEvenImage.draw(x,y,width, height);
}
