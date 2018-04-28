#ifndef _EYE_TRACKER_
#define _EYE_TRACKER_

#include "ofMain.h"
#include "ofxCvGrayscaleAdvanced.h"
#include "ofxOpenCv.h"
#include "eyeFinder.h"
#include "pupilFinder.h"
#include "glintFinder.h"
#include "EllipseCalcurator.h"
#include "homographyCalcurator.h"
#include "brightDarkFinder.h"
#include "thresholdCalculator.h"

typedef struct {
    bool bEyeFound;
    bool bGoodAlternation;
    bool bGlintInBrightEyeFound;
    bool bGlintInDarkEyeFound;
    bool bPupilFound;
} trackingState;


class eyeTracker {

public:

    eyeTracker();
    void        setup(int width, int height);
    void        update(ofxCvGrayscaleImage & grayImgFromCam, bool newFrame);
    ofPoint     getGlintPoint(int glintID);
    ofPoint     getVectorGlintToPupil(int glintID);            // final eye tracked position. (not gaze estimation)
    ofPoint     getPupil();

    //images
    ofxCvGrayscaleAdvanced            currentImg;
    ofxCvGrayscaleImage               brightEyeImg;
    ofxCvGrayscaleImage               darkEyeImg;
    ofxCvGrayscaleAdvanced            magCurrentImg;
    ofxCvGrayscaleAdvanced            smallCurrentImg;

    //finders
    eyeFinder                        eFinder;
    pupilFinder                      pFinder;
    glintFinder                      gFinder;

    //automatic thresholding for pupil finder and glint finder
    thresholdCalculator              thresCal;

    //eyefinding rectangle
    ofPoint                          pupilCentroid;
    ofRectangle                      targetRect;
    float              magRatio;
    int                w, h;
    bool               bFoundEye;

    //eye Tracking
    trackingState      tState;
    float              divisor;
    float              threshold_e;
    ofParameter<float> thresholdE{"Eye Threshold", -1, -1, 50};
    ofParameter<float> minBlobSize_e{"Minimum Eye Size", 5000, 0, 10000};
    ofParameter<float> maxBlobSize_e{"Maximum Eye Size", 15000, 0, 20000};
    float blobSize_e = 64;
    //ofParameter<float> blobSize_e{"Eye Size", 64, 0, 200}; //breaks too much stuff to be worth doing right now.

    //pupil Tracking
    bool                bFoundOne;
    bool            bUseAutoThreshold_p = false;
    float            threshold_p;
    ofParameter<float> thresholdP{"Pupil Threshold", 1, 0, 200};
    ofParameter<float> minBlobSize_p{"Minimum Pupil Size", 5000, 0, 10000};
    ofParameter<float> maxBlobSize_p{"Maximum Pupil Size", 15000, 0, 20000};

    //glint Tracking
    bool            bUseAutoThreshold_g = false;
    float            threshold_g;
    ofParameter<float> thresholdG{"Glint Threshold", 254, 0, 255};
    ofParameter<float> minBlobSize_g{"Minimum Glint Size", 30, 0, 1000};
    ofParameter<float> maxBlobSize_g{"Maximum Glint Size", 100, 0, 1000};

    //Bright/Dark Threshold
    brightDarkFinder    briDarkFinder;
    ofPoint             currentEyePoint;
    bool                bIsBrightEye;
    bool                bUseGlintinBrightEye;

    // Warp, Homography (angle of eye to camera)
    homographyCalcurator    homographyCal;


protected:

    bool    getBrightEyeDarkEye();

};

#endif //_EYE_TRACKER_
