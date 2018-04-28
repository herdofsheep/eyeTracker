
#include "testApp.h"
#include "stdio.h"

//--------------------------------------------------------------
testApp::testApp(){

}

//--------------------------------------------------------------
void testApp::setup(){

    //begin with trackingManager, which makes it easy to calibrate the software to pick up pupils and glints best for the viewing conditions.
	mode = MODE_TRACKING;
    //setup other modes/scenes
	TM.setup();
	CM.setup();

    //initialise the mousecursor smoothing vector
	eyeSmoothed.set(0,0,0);
    
    //setup button triggers for testing the eyetracking
	BT.setup("catch me!", 50,50,180,180);
	BT.setRetrigger(true);

}


//--------------------------------------------------------------
void testApp::update(){

    ofBackground( 30, 30, 30 );

	// update the tracking manager (and internally, its input manager)
	TM.update();

	// update the calibration manager
	CM.update();

	// record some points if we are in auto mode
	if (CM.bAutomatic == true && CM.bAmInAutodrive == true && CM.bInAutoRecording){
        
        //uses mouse input rather than eye input to check calibration mode's doing its job.
        if ( bMouseEyeInputSimulation ){
            CM.fitter.registerCalibrationInput(mouseX,mouseY);
            CM.inputEnergy = 1;
        }
        
        //If an eye is detected, use the pupilCentroid/glint difference determined by eyeTracker.getVectorGlintToPupil() (or just the pupil centroid if a glint isn't detected) to find the look position.
        else if ( TM.bGotAnEyeThisFrame() ){
            ofPoint trackedEye = TM.getEyePoint();
            CM.fitter.registerCalibrationInput(trackedEye.x,trackedEye.y);
            CM.inputEnergy = 1;
        }
        
	}
    
	if (!bMouseSimulation){
        //if the eyetacking has been calibrated by calibration manager
		if (CM.fitter.bBeenFit){

			ofPoint trackedEye;

			if (bMouseEyeInputSimulation) {
				trackedEye.x = mouseX;
				trackedEye.y = mouseY;
			} else {
                //set the eyeposition to where the eyeTracking manager reckons it is.
				trackedEye = TM.getEyePoint();
			}
            
            //set the cursor to the direction you're looking in.
			screenPoint = CM.fitter.getCalibratedPoint(trackedEye.x, trackedEye.y);
            //smooth the cursor's motion to reduce the effects of noise.
			eyeSmoothed.x = CM.smoothing * eyeSmoothed.x + (1-CM.smoothing) * screenPoint.x;
			eyeSmoothed.y = CM.smoothing * eyeSmoothed.y + (1-CM.smoothing) * screenPoint.y;
		}

	} else {
        //if using bMouseSimulation
		eyeSmoothed.x = mouseX;
		eyeSmoothed.y = mouseY;
	}

    //buttonTriggers in MODE_TEST listen to if the lookPosition is on top of them.
	if (mode == MODE_TEST){
		ofPoint pt = eyeSmoothed;
		if (BT.update(pt.x, pt.y)){
			BT.x = ofRandom(100,ofGetWidth()-100);
			BT.y = ofRandom(100,ofGetHeight()-100);
		}
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(225);
    
    //tracking mode draws the programs we want to see while adjusting image processing settings for eye, pupil and glint detection.
    if (mode == MODE_TRACKING){
        ofEnableAlphaBlending();
        ofSetColor(255, 255, 255, 120);
        TM.draw();
        TM.drawInput( 0, 0, TM.IM.width/4, TM.IM.height/4, TM.IM.width/4, 0, TM.IM.width/4, TM.IM.height/4, false );
        ofDisableAlphaBlending();
    }
    //draw calibration recording guides. TM.drawInput is optional, and can be distracting.
	if (mode == MODE_CALIBRATING)		         CM.draw(), TM.drawInput( (ofGetWidth()/2-TM.IM.width/8), 0, TM.IM.width/4, TM.IM.height/4, 0, 0, 0, 0, true );
    //draws buttonTriggers to play with the eyeTracking.
    if (mode == MODE_TEST)                       BT.draw(), CM.drawGUI();

	// draw a green dot to see how good the tracking is:
    if ( CM.fitter.bBeenFit || bMouseSimulation ){
		
		ofSetColor(0,255,0,120);
		ofFill();
		ofCircle(eyeSmoothed.x, eyeSmoothed.y, 20);
        
	}

	ofSetColor(255, 255, 255);
    
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){

    //keyboard controls to manage the program.
	switch (key){

		case	OF_KEY_RETURN:
			mode ++;
			mode %= 4; // number of modes (4 rather than 5 as MODE_COME_CLOSER isn't currently functioning).
			break;

		case	'm':
		case	'M':
			bMouseSimulation = !bMouseSimulation;//sets the looklocation to be found, and allocates mousePosition to lookLocation.
			break;


		case	'n':
		case	'N':
			bMouseEyeInputSimulation = !bMouseEyeInputSimulation;//assumes the lookLocation hasn't been found, so you can input couse data to calibration mode and smoothing to check training the calibrationManager is functioning correctly.
			break;


		case	'f':
		case	'F':
			ofToggleFullscreen();
			break;

		case	'c':
		case	'C':
			TM.setOriginalPosition(); //this resets brightEye/darkEye alternation, which I haven't worked with as I ahven't built the eyewriter hardware.
			break;

		case 'x':
		case 'X':
			TM.bOriginalPositon = false; //this adjusts brightEye/darkEye alternation, which I haven't worked with as I ahven't built the eyewriter hardware.
			break;


	}
    //passes keyPressed to calibrationManager.
	if (mode == MODE_CALIBRATING){
		CM.keyPressed(key);
	}
}
