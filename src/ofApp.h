#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

#include "ofxGui.h"
#include "ofxDDSPlayer.h"

enum {
    START, END, NONE,
};

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
		
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
    void mouseMoved(int x, int y);
	void windowResized(int w, int h);
	
	ofxKinect kinect;
	
	ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage depthImage, backgroundImage;
	
	
	int angle;
    
    ofRectangle roiRect;
    ofRectangle drawingRect;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
    
    bool mouseDown;
    int selectState;
    bool drawDebug, drawGui;
    
    float roiAverage;
    
    ofxDDSPlayer player;
    
    ofxPanel gui;
    ofParameter<float> currentFrame, frameStep;
    ofParameter<float> averageDist, minDist;
    ofParameter<ofVec4f> captureArea;
    
    deque<float> distanceValues;
    ofParameter<int> averageOver;
    ofParameter<float> rollingAverage;
    
    ofParameter<ofVec2f> frameStepRange;
    
    void editCaptureArea(ofVec4f &v);
    
    
    
};
