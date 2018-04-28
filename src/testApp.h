#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "trackingManager.h"
#include "calibrationManager.h"
#include "ButtonTrigger.h"


enum{

	MODE_TRACKING,	MODE_CALIBRATING,	MODE_TEST

};

class testApp : public ofBaseApp {

	public:

		testApp();
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);

		//----- scenes
		trackingManager			TM;
		calibrationManager		CM;
    
        //button utility for MODE_TEST
        buttonTrigger            BT;

        //smoothing so eyetracking isn't so glitchy
		ofPoint eyeSmoothed;
		ofPoint screenPoint;
    
        //keeps track of the mode we're in.
		int mode;
    
        //sets the looklocation to be found, and allocates mousePosition to lookLocation.
		bool bMouseSimulation = false;
        //assumes the lookLocation hasn't been found, so you can input couse data to calibration mode and smoothing to check training the calibrationManager is functioning correctly.
		bool bMouseEyeInputSimulation = false;

};

#endif
