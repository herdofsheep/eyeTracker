# eyeTracker

//////////////////////////////////////////////////////////////////////////
//        OpenFrameworks 0.9.8 stable branch, xCode version 9.0         //
//          Fixed for 2018 by Megan Bates, originally developed         //
//        by eyewriter.org, by members of Free Art and Technology     	//
//	(FAT), OpenFrameworks and the Graffiti Resarch Lab: Tempt1,	        //
//	 Evan Roth, Chris Sugrue, Zach Lieberman,Theo Watson and 	          //
//				James Powderly.                		                            //
//            github: https://github.com/eyewriter/eyewriter            //
//      required addons: ofxOpenCV, ofxVectorMath, ofxXmlSettings       //
//                       ofxGUI.                                        //
//      if using a ps3eye: ofxKinect, ofxPS3EyeGrabber                  //
//      if running the shells artwork: ofxAssimpModelLoader             //
//////////////////////////////////////////////////////////////////////////

//NOTES ON COMMENTING//
The original eyewriter code was a more extensive app with a keyboard, drawing and pong mode, 
but it did not run with openFrameworks 9.0. I have stripped and updated it. 
Check "tweaking/updatings eyewriter notes" for a runthrough of how I fixed and stripped the code. 
This program has more heavily altered code in testApp, calibrationManager, trackingManager, eyeTracker, 
eyeFinder, inputManager and pupilFinder. These files are more heavily commented and include explanations 
for what is going on by me (Megan). Other files are all nearly identical to the original eyeWriter program. 
All credit goes to eyewriter and I have not added additional commenting because I do not have as in depth 
knowledge of everything they are doing.

//TIPS AND TRICKS//
This program works better in a dark room. The haar tracking can pick up eyes more often if you're in fairly 
dark conditions. I tested and ran this with a PS3eye, using a webcam I couldn't get enough detail from the 
picture to detect pupils, let alone glints. That said, make sure your screen brightness is low or turn off 
glint detection (bool bFindGlints in trackingManager to false) in dark conditions as otherwise you will get 
different glints in different scenes from the computer screen.
My setup was a bright white LED mounted on the PS3eye. Holding your head and the camera very still, and quite 
close to your eye, are the way to get decent tracking. Make sure you pick up eyes consistently with your GUI 
setup, then adjust the GUI parameters until the pupil is the only thing picked up in the pupil tracker, and 
consistently.

//FUTURE DEVELOPMENT//
The eyetracking is still not particularly robust. I would like to improve the results by studying webgazer 
(https://webgazer.cs.brown.edu/), which worked excellently with no calibration and an ordinary webcam for me. 
My knowledge of java is limited and this would probably extend the project by the same length of time again! 
I hope users will develop and improve the eyeTracker, and build some exciting projects with it! Get in touch
if you make something with it, I would love to hear about it :)

// ADDITIONAL TIPS AND TRICKS
// If you aren't using PS3eye, go to tracking/inputManager.cpp and change the commenting at the top of the file.
// Not tracking eyes at all? try changing bRightEyeHaarFinder in Tracking/eyeTracker/eyeFinder.h to false and 
// bContourFinder to true.
// Still having troubles? Try the program in a darker space. It doesn't work as well in a bright room/outside.
// Make sure your screen brightness is low or turn off glint detection (bool bFindGlints in trackingManager to 
// false) in dark conditions as otherwise you will get different glints in different scenes from the computer screen.
// Don't have a bright LED attached to your camera so can't detect glints? Go to Tracking/trackingManager.h and 
// change bFindGlints to false.
// To make eyeWriter compile quickly, keep bLoadArtMode = false in testApp.h. If eyetracking is working, set it to 
// true to try out the shell gaze artwork.
// Troubles with calibration? try setting bMouseEyeInputSimulation in testApp.h to true to use mouse input instead of 
// eye input. If the calibration works with mouse input, you may not be tracking pupils very well. try to reduce their 
// size to just the pupil (not iris) or check they are picked up when you look at different parts of the screen.
// Can't work out how to use 'thresholds' in the GUI? Try setting bUseAutoTheshold_p, and _g to true in 
// Tracking/eyeTracker/eyeTracker.h to true.
// The GUI will auto load your settings if you press the save icon in the top of the GUI. Do try this if you're sick 
// of readjusting over and over.

 
