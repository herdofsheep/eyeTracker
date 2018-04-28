#include "ofMain.h"
#include "eyeTracker.h"
#include "inputManager.h"
#include "ofxXmlSettings.h"
#include "ofxGui.h"

class trackingManager {

public:
	trackingManager();

	void setup();
	void update();
	void draw();
	void drawInput(int xBright, int yBright, int wBright, int hBright, int xDark, int yDark, int wDark, int hDark, bool bSimple);
	void trackEyes();
	ofPoint getEyePoint();		// returns the tracked vector from glint to pupil centroid
	bool bGotAnEyeThisFrame();
	void setOriginalPosition();

    //---GUI
    ofxPanel gui;
    //Booleans
    ofxFloatSlider darken;
    ofxToggle _bUseDilate;
    ofxToggle _bUseGamma;
    ofxToggle _bUseContrast;
    ofxToggle _bUseBlur;
    ofxToggle _bUseAdaptiveThreshold;
    ofxToggle _bUseAdaptiveGauss;

    //ofParameter<float> gamma{"Gamma", start, min, max};
    //ofParameter<float> contrast{"Contrast", start, min, max};

	inputManager		IM;
	eyeTracker			tracker;
    ofVideoGrabber      videoGrabber;

	ofPoint				eyePoint;
	ofPoint				glintPoint[2];
	ofPoint				glintPupilVector;
	ofPoint				currentdrawPoint;

	ofImage				originalPositionB;
	ofImage				originalPositionD;
    ofxCvGrayscaleImage currentImage;
	bool				bOriginalPositon;
    bool                bFoundEye;
    bool                bFindGlints = true;

	float lastShutter, lastGain, lastExposure, lastBrightness, lastGamma;
};
