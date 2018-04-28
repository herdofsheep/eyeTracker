#ifndef _CALIBMANGER
#define _CALIBMANGER

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxLeastSquares.h"
#include "calibrationFileSaver.h"
#include "pointFitter.h"
#include "ofxGui.h"

class calibrationManager {

	public:

			void setup();
			void update();
			void draw();
        void drawGUI();
			void clear();
			void		start();
			void		stop();

		void keyPressed(int key);

		//this rectangle helps to communicate data to fitter.
		ofRectangle calibrationRectangle;

		//number of calibration points to get the training data for the eyeTracker. These can't currently be changed.
		int			nDivisionsWidth;
		int			nDivisionsHeight;

		//keeps track of where we are in the calibration grid
		float		xp, yp;
		int			pos;

		//these keep track of weighting for calibrations.
		float		menuEnergy;
		float		inputEnergy;
		float		autoPct;
		int			pt;

		//these keep track of where we are in the calibration process.
		bool		bPreAutomatic = false;
		bool		bAutomatic = false;
		bool		bAmInAutodrive = false;
		bool		bInAutoRecording = false;

		//these keep track of timings for animations and record times.
		float		startTime;
		float		recordTimePerDot;
		float		totalTimePerDot;

		//image and text to provide guidance to the user of the program.
		ofImage		calibrationInfo;
		ofTrueTypeFont font;

  	//---GUI
  	ofxPanel gui;
    //Slider
    ofxFloatSlider smoothing;
    ofxFloatSlider preTimePerDot;

		//booleans to help with what processes should be shown.
		bool	bDrawCalibrationGrid;
		bool	bDrawEyeInput;
		bool	bDrawRawInput;
		bool	bDrawMapOutput;

		//stores the calibration data
		calibrationFileSaver calibFileSaver;

		//maps the pupil positions to screen locations.
		pointFitter		fitter;


};

#endif
