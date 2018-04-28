#include "calibrationManager.h"

//--------------------------------------------------------------
void calibrationManager::setup(){

    //GUI
    gui.setup();
    //cursor lookLocation smoothing can be adjusted here to reduce jumpiness from noise.
    gui.add(smoothing.setup("Smoothing", 0.9, 0, 1));
    //adjusts calibration time dependent on how long you want to take to move your eye/mouse between recordings.
    gui.add(preTimePerDot.setup("Calibration Speed", 1, 0, 10));

  //image communicates advise on how to calibrate to the user.
	calibrationInfo.loadImage("images/calibrationInfo.png");
	font.loadFont("fonts/HelveticaNeueMed.ttf", 32);

  //number of calibration points to get the training data for the eyeTracker. These can't currently be changed.
	nDivisionsWidth = 4;
	nDivisionsHeight = 3;

  //keeps track of where we are in the calibration grid
	pos  = 0;

  //these keep track of weighting for calibrations.
	inputEnergy = 0;
  menuEnergy = 1;

  //these keep track of timings for animations and record times.
	startTime = ofGetElapsedTimef();
	recordTimePerDot = 0.4;
	totalTimePerDot = preTimePerDot + recordTimePerDot;
	autoPct = 0;

  //this rectangle helps to communicate data to fitter.
	calibrationRectangle.x = 0;
	calibrationRectangle.y = 0;
	calibrationRectangle.width = 1024;
	calibrationRectangle.height = 768;

  //fitter helps to translate our pupil data to a lookLocation point
	fitter.setup(nDivisionsWidth, nDivisionsHeight);

}

//--------------------------------------------------------------
void calibrationManager::start(){
  //starts calibration, if the manager is triggered.

	bAutomatic = true;
	bAmInAutodrive = true;
	startTime = ofGetElapsedTimef();

	fitter.startCalibration();
}

//--------------------------------------------------------------
void calibrationManager::stop(){
  //cancels the calibration and resets it.

	bAutomatic = false;
	bPreAutomatic = false;
	bAmInAutodrive = false;
	startTime = ofGetElapsedTimef();
	pos  = 0;

}

//--------------------------------------------------------------
void calibrationManager::update(){

  //make sure calibrationRectangle is the size of the screen.
	calibrationRectangle.x = 0;
	calibrationRectangle.y = 0;
	calibrationRectangle.width = ofGetWidth();
	calibrationRectangle.height = ofGetHeight();

	float widthPad = ofGetWidth() * 0.025f;
	float heightPad = ofGetHeight() * 0.025f;

  //shrinks calibrationRectangle so there is a border from the edge.
	calibrationRectangle.x += widthPad;
	calibrationRectangle.y += heightPad;
	calibrationRectangle.width -= widthPad*2;
	calibrationRectangle.height -= heightPad*2;

  //make fitter ignore noisy data points
	fitter.removeOutliers();

	totalTimePerDot = preTimePerDot + recordTimePerDot;

  //adjust 'energy', calibration weighting parameters
	if ((bAutomatic == true && bAmInAutodrive == true) || bPreAutomatic){
		menuEnergy = 0.94f * menuEnergy + 0.06f * 0.0f;
	} else {
		menuEnergy = 0.94f * menuEnergy + 0.06f * 1.0f;
	}

	// do the automatic recording management
	if (bAutomatic == true && bAmInAutodrive == true){
		int nPts = nDivisionsWidth * nDivisionsHeight;
		float totalTime = totalTimePerDot * nPts;

		if (ofGetElapsedTimef() - startTime > totalTime){		// calibration finish.
			bAmInAutodrive = false;
			bInAutoRecording = false;
			bPreAutomatic = false;
			fitter.calculate(calibrationRectangle);

		} else {

			float diffTime = ofGetElapsedTimef() - startTime ;
			pt = (int)(diffTime / totalTimePerDot);
			float diffDotTime = diffTime - pt * totalTimePerDot;
			// cout << diffTime << " " << pt <<  " " << diffDotTime << endl;
			if (diffDotTime < preTimePerDot){

				autoPct = (diffDotTime / preTimePerDot);
				bInAutoRecording = false;

			} else {
				autoPct = (diffDotTime - preTimePerDot) / recordTimePerDot;
				bInAutoRecording = true;
			}
			pos = pt;
		}
	}

	inputEnergy *= 0.98f;

  //find the location of our current calibration point
	int xx = (pos % nDivisionsWidth);
	int yy = (pos / nDivisionsWidth);

	bool bEven = false;
	if (yy % 2 == 0)  bEven = true;

  //translate the data we've gotten from calibration Rectangle to send to fitter.
	xp = bEven ? calibrationRectangle.x + ((float)calibrationRectangle.getWidth() / (float)(nDivisionsWidth-1)) * xx :
				 calibrationRectangle.x + (calibrationRectangle.getWidth() - ((float)calibrationRectangle.getWidth() / (float)(nDivisionsWidth-1)) * xx);

	yp = calibrationRectangle.y + calibrationRectangle.getHeight() - ((float)calibrationRectangle.getHeight() / (float)(nDivisionsHeight-1)) * yy;

  //data is pushed from testApp directly to fitter.
	fitter.update(pt, xp, yp);
}

//--------------------------------------------------------------
void calibrationManager::draw(){

	// draw a light grid:

	ofEnableAlphaBlending();
	ofSetColor(30, 30, 30, (int) (255 - 255 *  menuEnergy));
	ofRect(0,0,ofGetWidth(), ofGetHeight());

    ofSetColor(255, 255, 255, 40);
    for (int i = 0; i < nDivisionsWidth; i++){
        float xLine = calibrationRectangle.x + ((float)calibrationRectangle.getWidth() / (float)(nDivisionsWidth-1)) * i;
        ofLine(xLine, calibrationRectangle.y,xLine, calibrationRectangle.y + calibrationRectangle.getHeight());
    }

    for (int i = 0; i < nDivisionsHeight; i++){
        float yLine = calibrationRectangle.y + calibrationRectangle.getHeight() - ((float)calibrationRectangle.getHeight() / (float)(nDivisionsHeight-1)) * i;
        ofLine(calibrationRectangle.x,yLine, calibrationRectangle.x + calibrationRectangle.getWidth(),yLine);
    }

	//-----------------------------------------------------------------------
  //if we haven't yet calibrated the lookLocation
	if (fitter.bBeenFit != true){

		ofEnableAlphaBlending();

		float rad = 30;
		glLineWidth(4);
		ofSetColor(255, 255, 255, 100);
		ofLine (xp, yp-rad, xp, yp+rad);
		ofLine (xp-rad, yp, xp+rad, yp);;

		ofSetColor(255, 255, 255, 255);

		if (bAutomatic == true && bAmInAutodrive == true && bInAutoRecording){
		} else {
			ofCircle(xp, yp, rad*0.33);
		}

		ofFill();

    //draw white shrinking circles to help the user know where and when recording is taking place.
		if (bAutomatic == true && bAmInAutodrive == true){

			if (bInAutoRecording){

				ofSetColor(255, 0, 0, 200);
				ofCircle(xp, yp, 26);

				ofSetColor(255, 255,255);
				ofCircle(xp, yp, 5);

			} else {
				ofNoFill();
				ofSetColor(255, 255, 255, 150);
				ofCircle(xp, yp, 200 - 200* autoPct);
				ofFill();
			}
		}

		glLineWidth(1);
		ofFill();

    //while we're recoding, draw a red circle.
		if (!bAutomatic)
		if (inputEnergy > 0){
			ofSetColor(255, 0, 127, (int) (60*inputEnergy));
			ofCircle(xp, yp, rad * 3.5 );

			ofSetColor(255, 0, 127, (int) (150*inputEnergy));
			ofCircle(xp, yp, rad );
		}

		ofSetColor(255, 255, 255);
		//calibrationInfo.draw(100,100);
	}

	//-----------------------------------------------------------------------
  //if we're not currently calibrating, say whether the program is calibrated or not.
	if (!bAutomatic && !bPreAutomatic){

		if (!fitter.bBeenFit)	{
			ofSetColor(255, 0, 0,127);
			font.drawString("not calibrated", 100,100);
		} else {
			ofSetColor(0, 255, 0, 127);
			font.drawString("calibrated", 100,100);
		}
	}

  //if we're about to run the calibration, show an image to let the user know the controls.
	if (bPreAutomatic == true){
		calibrationInfo.draw(100,100);
//		cout << bPreAutomatic << endl;
	}

  //show fitter outcome
	fitter.draw();
}

//--------------------------------------------------------------
void calibrationManager::drawGUI(){

    //--- GUI
    ofSetColor(255);
    gui.setPosition(ofGetWidth()-200,0);
    gui.draw();

}

//--------------------------------------------------------------
void calibrationManager::clear(){
  //restart the calibration manager if the calibration is cancelled.
	pos = 0;
	fitter.clear();
}
//--------------------------------------------------------------
void calibrationManager::keyPressed(int key) {
  //when we press space, clear the fitter and start calibration over again.
	if (bPreAutomatic == true){
		if (key == ' '){
			bPreAutomatic = false;
			start();
		}
	}else if (bPreAutomatic == false && !bAmInAutodrive) {
		if (key == ' '){
			bPreAutomatic = true;
		}
	}else if (key == ' '){
		clear();
		stop();
	}
}

//--------------------------------------------------------------
