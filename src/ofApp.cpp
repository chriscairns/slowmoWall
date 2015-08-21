#include "ofApp.h"

inline ofVec2f ofMap(ofVec2f p, ofRectangle fromRect, ofRectangle toRect){
    
    float x = ofMap(p.x, fromRect.position.x, fromRect.width, toRect.position.x, toRect.width);
    float y = ofMap(p.y, fromRect.position.y, fromRect.height, toRect.position.x, toRect.height);
    return ofVec2f(x, y);
}

inline ofRectangle ofMap(ofRectangle r, ofRectangle fromRect, ofRectangle toRect){
    
    float x = ofMap(r.position.x, fromRect.position.x, fromRect.width, toRect.position.x, toRect.width);
    float y = ofMap(r.position.y, fromRect.position.y, fromRect.height, toRect.position.x, toRect.height);
    float w = ofMap(r.width, fromRect.position.x, fromRect.width, toRect.position.x, toRect.width);
    float h = ofMap(r.height, fromRect.position.y, fromRect.height, toRect.position.x, toRect.height);
    
    return ofRectangle(x, y, w, h);
}

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	kinect.setRegistration(true);
	kinect.init();
	kinect.open();
    
	grayImage.allocate(kinect.width, kinect.height);
    
    ofSetFrameRate(25);
    ofSetFullscreen(true);
	
	// zero the tilt on startup
	angle = 0;
    kinect.setDepthClipping();
    
    drawingRect.set(10, 10, 400, 300);
    mouseDown = false;
    selectState = NONE;
    
    //player.loadMovie("/Users/itg/Desktop/2015_08_20_hap_encodes/cig_light_hap_1280_720.mov");
    player.loadWithDirectory("/Users/itg/Desktop/2015_08_20_hap_encodes/frames_1080");
    
    drawDebug = true;
    drawGui = true;
    
    gui.setup("params");
    gui.add(currentFrame.set("current frame", 0, 0, player.getTotalNumFrames()));
//
    gui.add(averageDist.set("average dist", 0, 0, 255));
    gui.add(minDist.set("min dist", 0, 0, 255));
    
    gui.add(averageOver.set("average over", 2, 1, 20));
    gui.add(rollingAverage.set("rolling average", 0, 0, 255));
    
    gui.add(captureArea.set("capture area", ofVec4f(), ofVec4f(), ofVec4f(400)));
    captureArea.addListener(this, &ofApp::editCaptureArea);
    
    gui.add(frameStep.set("frame step", 0, 0, 100));
    gui.add(frameStepRange.set("frame step range", ofVec2f(0.1, 25), ofVec2f(), ofVec2f(40)));
}

//--------------------------------------------------------------
void ofApp::update() {
	
	ofBackground(100, 100, 100);

	
	kinect.update();
	
	if(kinect.isFrameNew()) {
		
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        grayImage.erode();
        grayImage.invert();

        grayImage.setROI(ofMap(roiRect, drawingRect, ofRectangle(0, 0, grayImage.getWidth(), grayImage.getHeight())));
		
        
        unsigned char *pix = grayImage.getRoiPixels();
        float sum = 0, length = grayImage.getROI().width * grayImage.getROI().height;
        if (length > 0) {
            float min = 255;
            for (int i = 0; i < length; i++) {
                sum+= pix[i];
                if (pix[i] < min) min = pix[i];
            }

            averageDist = sum / length;
            minDist = min;
            
            distanceValues.push_front(minDist);
            while (distanceValues.size() > averageOver) {
                distanceValues.pop_back();
            }
            
            float av = 0;
            for (float v : distanceValues) {
                av+= v;
            }

            rollingAverage = av / averageOver;
            
            frameStep = ofMap(rollingAverage, 0, 255, frameStepRange->x, frameStepRange->y, true);
            player.update(frameStep);
            currentFrame.set(player.getPosition());
        }
        //cout << "sum = " <<  sum  << endl;
	}
    
//    cout << "setting frame to " << int(currentFrame) << endl;
//    player.setFrame(int(currentFrame));
//    currentFrame+= ofMap(mouseX, 0, ofGetWidth(), 1, 40);
//    if (currentFrame > player.getTotalNumFrames()) {
//        currentFrame-= player.getTotalNumFrames();
//    }
////    ofSetWindowTitle(ofToString(currentFrame));
//    ofSetWindowTitle(ofToString(ofGetFrameRate(), 0) + " FPS");
//
//    player.update();
//	
}

//--------------------------------------------------------------
void ofApp::draw() {
	
	ofSetColor(255, 255, 255);
	
    if (drawDebug) {

        // draw from the live kinect
        kinect.drawDepth(drawingRect);
        
        grayImage.drawROI(420, 320, 300, 300);
        grayImage.resetROI();

        ofSetColor(255, 255, 0);
        ofNoFill();
        ofRect(roiRect);
        
        //cout << roiRect.position.x << " " << roiRect.width << " " << mouseDown << endl;
        
    }
    else {
        
        player.draw();
    }
    
    if (drawGui) gui.draw();
}



//--------------------------------------------------------------
void ofApp::exit() {
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
        case 's':
            backgroundImage.setFromPixels(kinect.getDepthPixelsRef());
            break;
        case 'f':
            ofToggleFullscreen();
            break;
		case 'd':
            drawDebug ^= true;
			break;
        case ' ':
            drawGui ^= true;
            break;

	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    
    if (drawDebug) {
        selectState = (selectState + 1) % (NONE + 1);
        
        if (selectState == START) {
            captureArea.set(ofVec4f(x, y, 0, 0));
            selectState++;
        }

    }

    mouseDown = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    mouseDown = false;
}

void ofApp::mouseMoved(int x, int y) {

    
    if (drawDebug) {
        if (selectState == END) {
            ofVec4f v(captureArea->x, captureArea->y, x - captureArea->x, y - captureArea->y);
            captureArea.set(v);
            editCaptureArea(v);
        }
    }

}

void ofApp::editCaptureArea(ofVec4f &v) {
    roiRect.set(captureArea->x, captureArea->y, captureArea->z, captureArea->w);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
