#include "trackingManager.h"
//--------------------------------------------------------------
trackingManager::trackingManager(){
}
//--------------------------------------------------------------
void trackingManager::setup(){

	IM.setup();

    //GUI (built from scratch by Megan, as ofxControlPanel is old and wouldn't work for me.)
        gui.setup();
        gui.add(darken.setup("Darken Image", 0, 0, 255 ));
    		//Add ofParameters declared in Artifact.h class into Gui
        gui.add(tracker.thresholdE);
				//only bother with these if bContourFinder in eyeFinder.h = true.
					//gui.add(tracker.minBlobSize_e);
					//gui.add(tracker.maxBlobSize_e);
        gui.add(tracker.thresholdP);
					gui.add(tracker.minBlobSize_p);
					gui.add(tracker.maxBlobSize_p);
        gui.add(tracker.thresholdG);
            gui.add(tracker.minBlobSize_g);
            gui.add(tracker.maxBlobSize_g);
        gui.add(tracker.pFinder.nErosions);
        gui.add(tracker.pFinder.nDilations);
        gui.add(tracker.pFinder.gamma);
        gui.add(tracker.pFinder.contrast);
        gui.add(tracker.pFinder.brightness);
        gui.add(tracker.pFinder.blur);
    		//buttons (booleans)
        gui.add(_bUseDilate.setup("Use Erosion and Dilation", true));
        gui.add(_bUseGamma.setup("Use Gamma", true));
        gui.add(_bUseContrast.setup("Use Contrast and Brightness", true));
        gui.add(_bUseBlur.setup("Use Blur", true));
        gui.add(_bUseAdaptiveThreshold.setup("Use Adaptive Threshold", true));
        gui.add(_bUseAdaptiveGauss.setup("Use Adaptive Gauss", true));
    		//use saved tracking settings
        gui.loadFromFile("settings.xml");

	//--- set up tracking
	printf("VideoWidth, VideoHeight = %d, %d \n", IM.width, IM.height);
	tracker.setup(IM.width, IM.height);

	//keep track of whether we can see an eye or not.
	bFoundEye = false;
	//initialise the eye lookLocation to 0,0,0.
	eyePoint.set(0,0,0);

	//'original position' works with DarkEye and BrightEye monitoring, which need the official eyeWriter hardware setup to be relevant, I think.
	bOriginalPositon = false;
	originalPositionB.allocate(IM.width, IM.height, OF_IMAGE_GRAYSCALE);
	originalPositionD.allocate(IM.width, IM.height, OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
void trackingManager::update(){

	//send our boolean choices in the GUI to eyeFinder, so it knows whether to process them or not as they're switched.
  tracker.pFinder.pushGUI(_bUseDilate, _bUseGamma, _bUseContrast, _bUseBlur,  _bUseAdaptiveThreshold, _bUseAdaptiveGauss);

	//--- eye tracking (on new frames)
	if (IM.bIsFrameNew){									// check new frame.
		trackEyes();
	}

    //adjust brightness of input. An attempt to fix that eyeTracking doesn't work well in a very bright room. Didn't help much for me!
        ofxCvGrayscaleImage img = *IM.grayImage;
        ofPixels pix = img.getPixels();

        for(int i = 0; i < img.getWidth() * img.getHeight(); i++){
                pix[i] -= MIN( darken, pix[i] ); //this makes sure it doesn't go under 0 as it will wrap to 255 otherwise.
        }

        currentImage.setFromPixels(pix);

    //--- update video/camera input
    IM.update( currentImage );



		//use the pupil-glint difference if we're using glintfinder (to reduce noise), else just use the pupil centroid.
    ofPoint pt;
    if ( bFindGlints ) pt = tracker.getVectorGlintToPupil(GLINT_BOTTOM_LEFT);
    else pt = tracker.getPupil();

		//only update the point if the vector was found
		//the function returns a 0,0 point if vector was not found
		if( pt.x != 0.0 || pt.y != 0.0 ){
			glintPupilVector = pt;
		}
}

//--------------------------------------------------------------
ofPoint	trackingManager::getEyePoint(){
	//returns the position vector of the pupil location, managed in eyeTracker.
	return eyePoint;
}

//--------------------------------------------------------------
bool trackingManager::bGotAnEyeThisFrame(){
	//returns if we've spotted an eye this frame or not, managed in eyeFinder.
	return bFoundEye;
}

//--------------------------------------------------------------
void trackingManager::trackEyes(){

	tracker.update( currentImage, IM.bIsFrameNew );
    ofPoint pt;

	bFoundEye	= tracker.bFoundEye;
    if ( tracker.gFinder.bFound && bFindGlints ){
        pt	= tracker.getVectorGlintToPupil(GLINT_BOTTOM_LEFT);
    }
    else pt = tracker.getPupil();

	//only update the point if the vector was found
	//the function returns a 0,0 point if vector was not found
	if( pt.x != 0.0 || pt.y != 0.0 ){
		eyePoint = pt;
	}
}
//--------------------------------------------------------------
void trackingManager::setOriginalPosition(){
	//'original position' works with DarkEye and BrightEye monitoring, which need the official eyeWriter hardware setup to be relevant, I think.
	//I don't get distinguishable bright and dark eyes, though they seem to need to alternate frame to frame, and the eyeWriter hardware uses a
	//LED flashing at the camera framerate so it would pick up alternate bright and dark images.
	if ((tracker.bIsBrightEye && IM.fcount == 0) || (!tracker.bIsBrightEye && IM.fcount ==1)) {		//Bright Eye => Left, Dark Eye =>Right
		originalPositionB.setFromPixels(IM.grayEvenImage.getPixels());
		originalPositionD.setFromPixels(IM.grayOddImage.getPixels());
	} else {
		originalPositionD.setFromPixels(IM.grayEvenImage.getPixels());
		originalPositionB.setFromPixels(IM.grayOddImage.getPixels());
	}

	bOriginalPositon = true;
}

//--------------------------------------------------------------
void trackingManager::draw(){

		// Draw Standard Input
		ofSetColor(255,255,255);
    drawInput( 0, 0, IM.width/4, IM.height/4, IM.width/4, 0, IM.width/4, IM.height/4, false );

		// Draw EyeFinder (locates and draws a rctangular region of interst around what it sees as an eye)
		tracker.eFinder.draw(0, IM.height/4+30, IM.width/2, IM.height/2, !tracker.tState.bEyeFound);

		// Draw Pupil Finder (finds elliptical shapes in the tweaked eye image. GUI needs to be used to tweak the eye Image
 		// to make sure these ellipses are the pupil as often as possible.)
		tracker.thresCal.drawPupilImageWithScanLine(IM.width/2 + 20, 0, tracker.pFinder.imgBeforeThreshold.width*2/3, tracker.pFinder.imgBeforeThreshold.height*2/3, tracker.pFinder.imgBeforeThreshold);
		tracker.pFinder.draw(IM.width/2 + 20, tracker.pFinder.imgBeforeThreshold.height*2/3 + 30, !tracker.tState.bPupilFound);

    // Draw Glint Finder, finds bright spots which will (hopefully) be the stationary point of reflection in the eye.
    tracker.gFinder.draw(IM.width/2 + 20, tracker.pFinder.imgBeforeThreshold.height*4/3 + 60, tracker.tState.bGlintInDarkEyeFound);
		//bright eye alternation not happening without hardware.
    //tracker.gFinder.drawGlintinBrightEye(IM.width/2 + tracker.pFinder.imgBeforeThreshold.width + 40, 255 * 2 + 60, !tracker.tState.bGlintInBrightEyeFound);

		// Draw BrightEye, DarkEye, zoomed in on the region of interest (eye).
		ofSetColor(255,255,255);
		tracker.brightEyeImg.draw(0, IM.height/4 + IM.height/2 + 60);
		tracker.darkEyeImg.draw(tracker.targetRect.getWidth(), IM.height/4 + IM.height/2 + 60);

		// Draw auto threshold Line Graph for bright/dark eye.
		tracker.briDarkFinder.drawAutoThresholdBrightnessGraph(0, IM.height/4 + IM.height/2 + 60, !tracker.tState.bGoodAlternation);

		// Draw brightness graph
		//int tempX = IM.width/2 + tracker.pFinder.imgBeforeThreshold.width + 40;
		//tracker.thresCal.drawBrightnessScanGraph(tempX, 0, tracker.pFinder.imgBeforeThreshold, false, tracker.threshold_p, tracker.threshold_g, "BrightnessScan/Horizontal");
		//tracker.thresCal.drawBrightnessScanGraph(tempX, 255 + 30, tracker.pFinder.imgBeforeThreshold, true, tracker.threshold_p, tracker.threshold_g, "BrightnessScan/Vertical");

    //DRAW GUI
    ofSetColor(255);
    gui.setPosition(ofGetWidth()-200,0);
    gui.draw();

}

//--------------------------------------------------------------
void trackingManager::drawInput(int xBright, int yBright, int wBright, int hBright, int xDark, int yDark, int wDark, int hDark, bool bSimple){

	//this allows the switching between bright and dark eyes, so every frame is checked alternately.
	if ( (tracker.bIsBrightEye && IM.fcount == 0) || (!tracker.bIsBrightEye && IM.fcount ==1) ) {		//Bright Eye => Left, Dark Eye =>Right
		IM.drawEvenFrame(xBright, yBright, wBright, hBright);
		IM.drawOddFrame(xDark, yDark, wDark, hDark);
	} else {
		IM.drawOddFrame(xBright, yBright, wBright, hBright);
		IM.drawEvenFrame(xDark, yDark, wDark, hDark);
	}

	ofEnableAlphaBlending();
	if (bOriginalPositon){
		ofSetColor(255, 255, 255, 100);
		originalPositionB.draw(xBright, yBright, wBright, hBright);
		originalPositionD.draw(xDark, yDark, wDark, hDark);
	}
	ofDisableAlphaBlending();

		//bSimple is a boolean for having more or less drawn by trakingManager depending on the mode.
		//tracking mode draws more information about the input, art mode has a very simple input so
		//eyetracking can be demonstated in documentation.
    if (!bSimple){
        ofDrawBitmapString("Input_Bright", xBright+1, yBright+hBright + 12);
        ofDrawBitmapString("Input_Dark", xDark+1, yDark+hDark + 12);
    }
}
