#include "pupilFinder.h"

//----------------------------------------------------
void pupilFinder::setup(int width, int height, float _magRatio, float IMwidth, float IMheight){

	//pass image information from input manager
	magRatio = _magRatio;
	inputWidth = IMwidth;
	inputHeight = IMheight;

	//initialise image size, thresholding and ellipsefinder.
	currentImg.allocate((int) (width * magRatio), (int) (height * magRatio));
	imgBeforeThreshold.allocate((int) (width * magRatio), (int) (height * magRatio));
	ellipseCal.setup((int) (width * magRatio), (int) (height * magRatio));

	//adaptive thresholding parameters
	offset = 20;
	blocksize = 200;

}
//----------------------------------------------------
void pupilFinder::pushGUI( bool _bUseDilate, bool _bUseGamma, bool _bUseContrast, bool _bUseBlur, bool _bUseAdaptiveThreshold, bool _bUseAdaptiveGauss){
		//take values from trackingmanager GUI for image manipulation
		bUseDilate = _bUseDilate;
    bUseContrast = _bUseContrast;
    bUseBlur = _bUseBlur;
    bUseGamma = _bUseGamma;
    bUseAdaptiveThreshold = _bUseAdaptiveThreshold;
    bGauss = _bUseAdaptiveGauss;
}
//----------------------------------------------------
bool pupilFinder::update(ofxCvGrayscaleAdvanced & _currentImg, int threshold, int minBlobSize, int maxBlobSize, ofRectangle & _targetRect){

	//pupil finding rectangle starting parameters
	targetRect.x = _targetRect.x;
	targetRect.y = _targetRect.y;
	targetRect.width = _targetRect.getWidth();
	targetRect.height = _targetRect.getHeight();

	currentImg = _currentImg;

	//apply image manipulation effects for better tracking from GUI
	if (bUseGamma) currentImg.applyMinMaxGamma(gamma);
	if (bUseBlur) currentImg.blur(blur);
	if (bUseContrast) currentImg.applyBrightnessContrast(brightness,contrast);
	if (bUseAdaptiveThreshold) currentImg.adaptiveThreshold(blocksize, offset, true, bGauss);
	else currentImg.threshold(threshold, true);
	if (bUseDilate){

		int maxOperations	= MAX(nDilations, nErosions);

		for (int i = 0; i < maxOperations; i++) {
			if (i < nErosions)	currentImg.erode();
			if (i < nDilations)	currentImg.dilate();
		}
	}

	//see what effect GUI is making with a before/after.
	imgBeforeThreshold = currentImg;

	//openCV 's contourFinder spots the ellipse where the pupil is.
	int num = contourFinder.findContours(currentImg, minBlobSize, maxBlobSize, 1, true, true);

	//if we've found a blob, send it to ellipseFinder.
	if (num > 0) {

		resampler.resample(contourFinder.blobs[0], 0.1, 10);

		foundPupilBlob = contourFinder.blobs[0];

		//if the ellipse calculator thinks the blob looks like a pupil,
		if(ellipseCal.update(foundPupilBlob)){
			currentPupilCenter = ellipseCal.currentEyePoint / magRatio;
			return true;
		}
		else {
			return false;
		}
	}
}

//----------------------------------------------------
void pupilFinder::draw(float x, float y, bool bError){

	ofPushMatrix();
	ofTranslate(x, y, 0);

	ofEnableAlphaBlending();

	//show our manipulated image, with where we think the pupil is on top.
	ofSetColor(255,255,255, 80);
	currentImg.draw(0, 0);
	foundPupilBlob.draw(0, 0);
	ellipseCal.draw(0, 0);

	//outline the pupil
	ofSetColor(0, 0, 255, 255);
	if (contourFinder.blobs.size() > 0) {
		ofBeginShape();
		for (int i = 0; i < contourFinder.blobs[0].pts.size(); i++) {
			ofVertex(contourFinder.blobs[0].pts[i].x, contourFinder.blobs[0].pts[i].y);
		}
		ofEndShape(true);
	}

	ofDisableAlphaBlending();

	//describe what the image is so we know which one is the pupilFinder.
	ofSetColor(255, 255, 255);
	ofDrawBitmapString("pupilFinder/EllipseFit", 1, currentImg.height + 12);

	//let the user know if pupils aren't being found.
	if (bError) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("Error, can't find pupil", 200, currentImg.height + 12);
	}

	ofPopMatrix();

}

//----------------------------------------------------
