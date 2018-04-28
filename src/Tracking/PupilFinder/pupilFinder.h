#ifndef _PUPIL_FINDER_
#define _PUPIL_FINDER_

#include "ofMain.h"
#include "ofxCvGrayscaleAdvanced.h"
#include "ofxOpenCv.h"
#include "EllipseCalcurator.h"
#include "eyeFinder.h"
#include "blobResampler.h"

class pupilFinder {

public:

	void setup(int width, int height, float _magRatio, float IMwidth, float IMheight);
    void pushGUI( bool _bUseDilate, bool _bUseGamma, bool _bUseContrast, bool _bUseBlur, bool _bUseAdaptiveThreshold, bool _bUseAdaptiveGauss);
	bool update(ofxCvGrayscaleAdvanced & _currentImg, int threshold, int minBlobSize, int maxBlobSize, ofRectangle & _targetRect);
	void draw(float x, float y, bool bError);

	ofxCvGrayscaleAdvanced	currentImg;
	ofxCvGrayscaleImage		imgBeforeThreshold;			// only for displaying in tracking mode
	ofxCvContourFinder		contourFinder;
	ofxCvBlob				foundPupilBlob;
	ellipseCalcurator		ellipseCal;

	//fixes up blobs contourFinder locates
	blobResampler			resampler;

	float		magRatio;
	float		inputWidth;
	float		inputHeight;

	//image processing, pushed to the trackingManager GUI.
	//gamma darkens darks spots in the image
	bool		bUseGamma = false;
	ofParameter<float> gamma{"Gamma", 0.59f, 0, 2};
	bool		bUseDilate;
	ofParameter<int> nErosions{"Number Of Erosions", 9, 0, 30};
	ofParameter<int> nDilations{"Number Of Dilations", 15, 0, 30};

  //contrast increases difference between dark and light spots
  bool        bUseContrast = false;
  ofParameter<float> contrast{"Contrast", 0.665, 0, 1};
	ofParameter<float> brightness{"Brightness", 0.6, 0, 1};
	bool		bUseBlur = false;
	ofParameter<int> blur{"Blur", 30, 0, 50};

	//adaptive threshold binarises the image, splits it into black and white zones.
	bool		bUseAdaptiveThreshold = false;
	int			offset;
	int			blocksize;
	bool		bGauss = false;

	//condtion for choosing pupil
	//maxWidth and maxHeight uses for blob detection.
	float		maxWidth;
	float		maxHeight;
	bool		bUseCompactnessTest;
	float		maxCompactness;

	//translates the pupil location to parameters we can use- ofVec and ofRec.
	ofPoint			currentPupilCenter;
	ofRectangle	targetRect;

};

#endif /*_PUPIL_FINDER_*/
