//////////////////////////////////////////////////////////////////////////
//                                                                      //
//          WELCOME TO THE STRIPPED AND (BARELY) FUNCTIONAL EYE         //
//              TRACKING OPENSOURCE OPENFRAMEWORKS PROJECT!             //
//                                                                      //
//        OpenFrameworks 0.9.8 stable branch, xCode version 9.0         //
//          Fixed for 2018 by Megan Bates, originally developed         //
//        by eyewriter.org, by members of Free Art and Technology       //
//    (FAT), OpenFrameworks and the Graffiti Resarch Lab: Tempt1,       //
//        Evan Roth, Chris Sugrue, Zach Lieberman,Theo Watson and       //
//                             James Powderly.                          //
//            github: https://github.com/eyewriter/eyewriter            //
//      required addons: ofxOpenCV, ofxXmlSettings, ofxGUI.             //
//      if using a ps3eye: ofxKinect, ofxPS3EyeGrabber                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

                            //TIPS & TRICKS//
// If you aren't using PS3eye, go to tracking/inputManager.cpp and change the commenting at the top of the file.
// Not tracking eyes at all? try changing bRightEyeHaarFinder in Tracking/eyeTracker/eyeFinder.h to false and bContourFinder to true.
// Still having troubles? Try the program in a darker space. It doesn't work as well in a bright room/outside.
// Make sure your screen brightness is low or turn off glint detection (bool bFindGlints in trackingManager to false) in dark conditions as otherwise you will get different glints in different scenes from the computer screen.
// Don't have a bright LED attached to your camera so can't detect glints? Go to Tracking/trackingManager.h and change bFindGlints to false.
// To make eyeWriter compile quickly, keep bLoadArtMode = false in testApp.h. If eyetracking is working, set it to true to try out the shell gaze artwork.
// Troubles with calibration? try setting bMouseEyeInputSimulation in testApp.h to true to use mouse input instead of eye input. If the calibration works with mouse input, you may not be tracking pupils very well. try to reduce their size to just the pupil (not iris) or check they are picked up when you look at different parts of the screen.
// Can't work out how to use 'thresholds' in the GUI? Try setting bUseAutoTheshold_p, and _g to true in Tracking/eyeTracker/eyeTracker.h to true.
// The GUI will auto load your settings if you press the save icon in the top of the GUI. Do try this if you're sick of readjusting over and over.

#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
    ofSetupOpenGL(1024,768,OF_FULLSCREEN);
	//ofSetupOpenGL(&window, 1000,700, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of the app
	// can be OF_WINDOW or OF_FULLSCREEN
	ofRunApp( new testApp());

}
