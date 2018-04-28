#include "eyeFinder.h"

//locates eyes in input image, draws a rectangle where it thinks the eye is and zooms
//into this region of interest.
//------------------------------------------------------------------------------------
void eyeFinder::setup(int width, int height, int divisor){

    w = width;                    // width from inputManager (640 normally)
    h = height;                    // height from inputManager (480 normally)
    div = divisor;                // divisor to get smaller image (default 2)

    wdiv = w / div;
    hdiv = h / div;

    //NOTE: BY RIGHT, I MEAN RIGHT OF SCREEN. THIS IS THE USER's LEFT EYE. IT IS THE RIGHT EYE ON THE SCREEN.
    if (bRightEyeHaarFinder)  eyeFinderRight.setup("trackingData/ojoR.xml"); //use ojoL.xml if you would prefer to track the left eye.

    //setup small images in the region of interest. To be occupied by comparison images to track changes
    previousImg.allocate(wdiv, hdiv);
    diffImg.allocate(wdiv, hdiv);
    currentImg.allocate(wdiv, hdiv);

    //parameters for checking whether the eyeFinding rectangle is a good shape to be an image of an eye.
    maxWidth = 50000;
    maxHeight = 50000;

    minSquareness = 0.5;

}

//------------------------------------------------------------------------------------
bool eyeFinder::update(ofxCvGrayscaleAdvanced & _currentImg, float threshold, int _targetHeight, int minBlobSize_e, int maxBlobSize_e, bool newFrame){

    currentImg = _currentImg;            // _currentImg is already small(width&height are devided by div) from eyeTracker.

    rectSize = 0;

    // get diff Image;
    diffImg.absDiff(currentImg, previousImg);
    diffImg.threshold(threshold, false);

    //keeps track of if we have found an eye
    bFoundOne = false;

    //////////////////////////////////
    //      EYEFINDING METHOD:      //
    //        LOOK FOR EYES         //
    //////////////////////////////////
    if (newFrame) {

        offsetROI.x = findingRect.x * div;
        offsetROI.y = findingRect.y * div;
        offsetROI.width = findingRect.getWidth() * div;
        offsetROI.height = findingRect.getHeight() * div;

        //search inside of findingRect for eyes!
        diffImg.setROI(findingRect);

        //ContourFinder eyeDetection
        if( bContourFinder ){
            blobsSize = contourFinder.findContours(diffImg, minBlobSize_e, maxBlobSize_e, 20, true, true);
            for (int k = 0; k < blobsSize; k++){
                eyeBorder = contourFinder.blobs[k].boundingRect;
            }
        }

        //HaarFinder eyeDetection
        else if( bRightEyeHaarFinder ){
            eyeFinderRight.setScaleHaar(1.2);
            eyeFinderRight.findHaarObjects(currentImg);
            if( eyeFinderRight.blobs.size() > 0 ) {
                blobsSize = eyeFinderRight.blobs.size();
                for (int k = 0; k < blobsSize; k++){
                    eyeBorder = eyeFinderRight.blobs[k].boundingRect;
                }
            }
        }

        diffImg.resetROI();
    }


    //////////////////////////////////
    //      TARGETRECT: CHECK       //
    //      EYEBORDER RECT AND      //
    //      FIND ITS CENTROID.      //
    //////////////////////////////////
    if (newFrame) {
        for(int k = 0; k < blobsSize; k++){

            rectSize += eyeBorder.getWidth();
            if (rectSize > 0) bFoundOne = true;

            // bounding rect width & height test.
            if (eyeBorder.getWidth() > maxWidth/div || eyeBorder.getHeight() > maxHeight/div) {
                continue;
            }

            //ignore rectangular blobs
            float ratio = eyeBorder.getWidth() < eyeBorder.getHeight() ?
            eyeBorder.getWidth() / eyeBorder.getHeight() :
            eyeBorder.getHeight() / eyeBorder.getWidth();

            if( ratio > minSquareness ) {

                bFoundOne = true;

                eyeBorder.x = eyeBorder.x * div + offsetROI.x;
                eyeBorder.y = eyeBorder.y * div + offsetROI.y;
                eyeBorder.width = eyeBorder.getWidth() * div;
                eyeBorder.height = eyeBorder.getHeight() * div;

                //find the eye bounding rect using either Haar method or contourFinder method.
                if( bRightEyeHaarFinder && !bContourFinder ){

                    centroid = (eyeFinderRight.blobs[k].centroid + ofPoint(findingRect.x, findingRect.y)) * div;
                    foundBlob = eyeFinderRight.blobs[k];

                }
                else if( bContourFinder && !bRightEyeHaarFinder ){

                    centroid = (contourFinder.blobs[k].centroid + ofPoint(findingRect.x, findingRect.y)) * div;
                    foundBlob = contourFinder.blobs[k];

                }

                break;
            }
        }
    }


    previousImg = currentImg;

    //let the program know if we have an eye this frame.
    return bFoundOne;
}

//------------------------------------------------------------------------------------
void eyeFinder::draw(float x, float y, float width, float height, bool bError){

    ofNoFill();

    ofEnableAlphaBlending();
    ofSetColor(255,255,255, 80);

    //draw both the eye frame, and 'diffImg'- how it has changed since the previous frame.
    currentImg.draw(x, y, width, height);
    diffImg.draw(x, y, width, height);

    ofNoFill();

    ofSetColor(255,0,0, 130);

    //show the bounding rect for where the program thinks it has found an eye.
    ofRect(x + eyeBorder.x * width/w, y + eyeBorder.y * height/h,
           eyeBorder.getWidth() * width/w, eyeBorder.getHeight() * height/h);

    float    targetRectTempX = centroid.x - (targetWidth/2);
    float    targetRectTempY = centroid.y - (targetHeight/2);

    //blue rectangle surrounds eye which will be copied and magnified.
    if (bFoundOne){
        ofSetColor(0,0,255, 150);
        ofRect(x + targetRectTempX * width/w, y + targetRectTempY * height/h,
               targetWidth  * width/w, targetHeight * height/h);
    }

    //red rectangle frames area to search for eyes.
    ofSetColor(255, 0, 0,100);
    ofRect(findingRect.x * div * width/w + x, findingRect.y * div * height/h + y, findingRect.getWidth() * div * width/w, findingRect.getHeight() * div * height/h);

    if (bFoundOne){
        ofSetColor(255,255,255);
        foundBlob.draw(x + offsetROI.x * width / w, y + offsetROI.y * height / h);
    }

    ofDisableAlphaBlending();

    ofSetColor(255, 255, 255);
    ofDrawBitmapString("EyeFinder", x + 1, y + height + 12);

    //let us know if the eye is not being found. See methods in main.h to reduce this.
    if (bError) {
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("Error, can't find eye", x + 100, y + height + 12);
    }

}

//------------------------------------------------------------------------------------
