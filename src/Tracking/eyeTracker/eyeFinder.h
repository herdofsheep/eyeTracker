#ifndef _EYE_FINDER_
#define _EYE_FINDER_

#include "ofMain.h"
#include "ofxCvGrayscaleAdvanced.h"
#include "ofxOpenCv.h"
#include "ofxCvHaarFinder.h"

class eyeFinder  {

public:
    void setup(int width, int height, int divisor = 1);
    bool update(ofxCvGrayscaleAdvanced & _currentImg, float threshold, int _targetHeight, int minBlobSize_e, int maxBlobSize_e, bool newFrame);
    void draw(float x, float y, float width, float height, bool bError);

    //eye finding methods
    ofxCvContourFinder            contourFinder;
    ofxCvHaarFinder               eyeFinderRight;

    //image change comparison method.
    ofxCvGrayscaleAdvanced        currentImg;
    ofxCvGrayscaleAdvanced        previousImg;
    ofxCvGrayscaleAdvanced        diffImg;

    //contourFinder method shows the object it thinks is an eye
    ofxCvBlob                        foundBlob;

    //finding rectangle parameters
    ofRectangle      offsetROI;
    ofRectangle      eyeBorder;
    ofPoint          centroid;
    float            w, h, div;
    float            wdiv, hdiv;
    int              targetWidth,    targetHeight;
    int              rectSize;
    int              blobsSize;

    //eyeFinder method keeper. always keep one true and the other false.
    bool            bContourFinder = false;
    bool            bRightEyeHaarFinder = true;

    //keep track of what state of tracking we have achieved
    bool            bBadFrame;
    bool            bFoundOne;
    bool            bFirstFrame;

    //Filters to achieve best pupil detection
    bool            bUseGamma;
    float           gamma;
    bool            bUseDilate;
    int             nErosions, nDilations;
    bool            bUseBlur;
    float           blur;
    bool            bUseContrast;
    float           contrast;
    float           brightness;

    // Shape Filter to check for eye detection area.
    float           maxCompactness;
    bool            bUseLongShortTest;
    float           minLongShortRatio;
    CvBox2D         box;
    float           maxWidth;
    float           maxHeight;
    float           minSquareness;


private:

    ofRectangle    findingRect;

};

#endif /* _EYE_FINDER_ */
