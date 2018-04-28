#include "eyeTracker.h"
//--------------------------------------------------------------
eyeTracker::eyeTracker() {

    //keep track of if we can see an eye this frame
    bFoundOne    = false;

}

//----------------------------------------------------
void eyeTracker::setup(int width, int height) {

    w = width;                    // input manager width
    h = height;                    // input manager height
    magRatio = 4;                // for more accurate detection.
    divisor = 4;                // for eyeFinder, make the picture smaller to reduce computation

    printf("VideoWidth, VideoHeight = %d, %d \n", w, h);

    //initialisation of tracking images, and eye locating rectangle.
    currentImg.allocate(w, h);
    currentEyePoint = ofPoint(0, 0);

    targetRect.x = 0;
    targetRect.y = 0;
    targetRect.height = blobSize_e;
    targetRect.width = (targetRect.height)*2;

    //set correct image sizes
    magCurrentImg.allocate(targetRect.width * magRatio, targetRect.height * magRatio);
    smallCurrentImg.allocate(w/divisor, h/divisor);
    brightEyeImg.allocate(targetRect.width, targetRect.height);
    darkEyeImg.allocate(targetRect.width, targetRect.height);

    //tracking requires finding 3 separate features from the raw image: eys, pupils, and glints (bright reflective points)
    eFinder.setup(w, h, divisor);
    pFinder.setup(targetRect.width, targetRect.height, magRatio, w, h);
    gFinder.setup(targetRect.width, targetRect.height, magRatio, w, h);

    //automatic threshold calculator (what brightness is good for spotting pupils and glints)
    //and homography (angle of the eye to the camera)
    thresCal.setup(targetRect.width, targetRect.height, divisor);
    homographyCal.setup(100, 50, targetRect.width * magRatio, targetRect.height * magRatio);

    //initialise statuses for what the program has successfully found
    bUseGlintinBrightEye = false;
    bFoundEye = false;
    tState.bEyeFound = false;
    tState.bGoodAlternation = false;
    tState.bGlintInBrightEyeFound = false;
    tState.bGlintInDarkEyeFound = false;
    tState.bPupilFound = false;

}

//----------------------------------------------------
void eyeTracker::update(ofxCvGrayscaleImage & grayImgFromCam, bool newFrame) {

    // initialize status, keep status for pupil & glint.
    tState.bEyeFound = false;
    tState.bGoodAlternation = false;

    // get the image from input manager.
    currentImg.setFromPixels(grayImgFromCam.getPixels());

    // get the small size image to find eye position.
    if (divisor !=1) smallCurrentImg.scaleIntoMe(currentImg, CV_INTER_LINEAR);

    // get the eye position
    bFoundOne = eFinder.update(smallCurrentImg, thresholdE, blobSize_e, minBlobSize_e, maxBlobSize_e, newFrame);

    //update eyeSize with GUI
    targetRect.height = blobSize_e;
    targetRect.width = (targetRect.height)*2;

    if (eFinder.centroid.x > w - (targetRect.getWidth()/2) || eFinder.centroid.x < (targetRect.getWidth()/2) ||
        eFinder.centroid.y > h - (targetRect.getHeight()/2) || eFinder.centroid.y < (targetRect.getHeight()/2)){
        bFoundOne = false;
    }

    bFoundEye = false;
    bool bFoundPupil = false;

    //update the eye trackingState.
    tState.bEyeFound = bFoundOne;

    //if we've found an eye:
    if (bFoundOne){

        targetRect.x = eFinder.centroid.x - (targetRect.getWidth()/2);
        targetRect.y = eFinder.centroid.y - (targetRect.getHeight()/2);

        // make the eye image big.
        currentImg.setROI(targetRect);

        if (magRatio != 1) {
            magCurrentImg.scaleIntoMe(currentImg, CV_INTER_CUBIC);            // magnify by bicubic
        } else {
            magCurrentImg.setFromPixels(currentImg.getRoiPixels());
        }

        currentImg.resetROI();

        // get current bright eye image & dark eye image, and compare current status with past status.
        bool    bWasBrightEye = bIsBrightEye;
        bIsBrightEye = getBrightEyeDarkEye();
        if (bWasBrightEye != bIsBrightEye) tState.bGoodAlternation = true;

        // get glint position in a bright eye image, if needed. <= should be here..? think about it.
        if (bIsBrightEye && bUseGlintinBrightEye) {
            thresCal.update(smallCurrentImg, eFinder.diffImg, currentImg, targetRect, true);            // update threshold for glint in bright eye.
            tState.bGlintInBrightEyeFound = gFinder.findGlintCandidates(magCurrentImg, thresCal.getGlintThreshold(true), minBlobSize_g, maxBlobSize_g, true);
        }

        // find Pupil image again with the big eye image. (try double ellipse fit later?)
        if (!bIsBrightEye){

            // get the averages of pupil & white part.
            if (bUseAutoThreshold_g || bUseAutoThreshold_p){
                thresCal.update(smallCurrentImg, eFinder.diffImg, currentImg, targetRect, false);
            }

            if (bUseAutoThreshold_g) threshold_g = thresCal.getGlintThreshold(false);
            else threshold_g = thresholdG;

            // get glint position with dark eye image.
            gFinder.update(magCurrentImg, threshold_g, minBlobSize_g, maxBlobSize_g, bUseGlintinBrightEye);
            bFoundPupil = pFinder.update(magCurrentImg, threshold_p, minBlobSize_p, maxBlobSize_p, targetRect);

            tState.bGlintInDarkEyeFound = gFinder.bFound;

            //we can use the automatic method to decide what counts as a pupil or not.
            if (bUseAutoThreshold_p) threshold_p = thresCal.getPupilThreshold();
            else threshold_p = thresholdP;

            //update the pupil trackingState.
            tState.bPupilFound = bFoundPupil;

            //if we've found the pupil, let the rest of the program know we've found one.
            if (bFoundPupil){
                pupilCentroid = pFinder.currentPupilCenter;
                bFoundEye = true;
            }
        }
    }
}

//----------------------------------------------------
bool eyeTracker::getBrightEyeDarkEye() {
  //DarkEye and BrightEye monitoring, which need the official eyeWriter hardware setup to be relevant, I think.
	//I don't get distinguishable bright and dark eyes, though they seem to need to alternate frame to frame, and the eyeWriter hardware uses a
	//LED flashing at the camera framerate so it would pick up alternate bright and dark images.
    bool bBright = briDarkFinder.getBrightEyeDarkEye(currentImg, eFinder.eyeBorder);

    currentImg.setROI(targetRect);

    if (bBright) brightEyeImg.setFromPixels(currentImg.getRoiPixels());
    else darkEyeImg.setFromPixels(currentImg.getRoiPixels());

    currentImg.resetROI();
    return bBright;
}

//----------------------------------------------------
ofPoint eyeTracker::getGlintPoint(int glintID) {
    //rePhrasing of glintpoint to make it easier to call higher up
    return gFinder.getGlintPosition(glintID);

}

//----------------------------------------------------
ofPoint    eyeTracker::getVectorGlintToPupil(int glintID) {
  //finds glintpoint-pupilPoint to have a more stable relative pupil position for the eyetracking.
  //returns 0 if either is not found, so the program won't break if pupils or glints aren't found.
    if (pupilCentroid.x != 0 && gFinder.bFound == true){
        return (pupilCentroid - getGlintPoint(glintID));
    }
    return ofPoint(0,0);
}

//----------------------------------------------------
ofPoint    eyeTracker::getPupil() {
    //alternative to getvectorglintToPupil, just using pupil centroid,
    //in case it is difficult to find glints with your setup.
    if (pupilCentroid.x != 0){
        return (pupilCentroid);
    }
    return ofPoint(0,0);
}
