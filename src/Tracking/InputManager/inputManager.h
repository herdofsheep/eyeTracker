#ifndef _INPUT_MANAGER
#define _INPUT_MANAGER

#include "ofMain.h"
#include "ofxOpenCv.h"

#ifdef TARGET_OSX
#endif

class inputManager {

public:
	inputManager();

	void setup();
	void update( ofxCvGrayscaleImage currentImage );
	void drawOddFrame(int x, int y, int width, int height);
	void drawEvenFrame(int x, int y, int width, int height);

	//camera management
  ofVideoGrabber    cam;
	ofVideoPlayer			vidPlayer;
	float							playposition;
	//frame management
	bool					bIsFrameNew;
	int						fcount;

	//images management
	ofxCvColorImage				colorImg;
  ofxCvGrayscaleImage   grayImg;
  ofxCvGrayscaleImage*  grayImage;
	ofxCvGrayscaleImage		grayOddImage;
	ofxCvGrayscaleImage		grayEvenImage;
	int						width,height;

	//mirroring
	bool					flipX;
	bool					flipY;
};

#endif /* _INPUT_MANAGER */
