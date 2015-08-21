//
//  ofxDDSPlayer.h
//  slowmoWall
//
//  Created by itg on 21/08/2015.
//
//

#pragma once

#include "DDSTexture.h"
#include <dirent.h>

class ofxDDSPlayer {
    
    vector<string> dds;
    vector< shared_ptr<DDSTexture> > texes;
    int width, height;
    static const int NFBOS = 2;
    ofFbo fbos[NFBOS];
    ofShader mixShader;
    ofMesh mesh;
    
    float playhead;
    int nframes;
    
public:
    ofxDDSPlayer() {}
    
    void setup(int width, int height) {
        
        for (int i = 0; i < NFBOS; i++) {
            fbos[i].allocate(width, height);
        }
        
        string fragShader = "#version 120 \n \
        uniform sampler2DRect tex1, tex2; \
        uniform float q; \
        varying vec2 texc; \
        void main() { \
        vec2 p = gl_FragCoord.xy; \
        p.y = " + ofToString(height) + ".0 - p.y; \
        vec3 c1 = texture2DRect(tex1, p).xyz; \
        vec3 c2 = texture2DRect(tex2, p).xyz; \
        gl_FragColor = vec4(c1 * (1.0 - q) + c2 * q, 1.0); \
        }";
        
        string vertShader = "#version 120 \n \
        varying vec2 texc; \
        void main() {\
        gl_Position = ftransform();\
        texc = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0); \
        }";
        
        mixShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
        mixShader.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
        mixShader.linkProgram();
        
        mesh = ofMesh::plane(1920, 1080);
        
        playhead = 0;
        this->width = width;
        this->height = height;
        

    }
    
    void loadWithDirectory(string path) {
        
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (path.c_str())) != NULL) {
            
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                string file = ent->d_name;
                if(file.find(".dds")!=-1) {
                    dds.push_back(file);
                }
                            }
            closedir (dir);
        } else {
            printf("Error, could not list directory\n");
        }
        
        sort(dds.begin(), dds.end());
        
        for (string file : dds) {
            shared_ptr<DDSTexture> tex(new DDSTexture());
            tex->load(path + string("/") + file);
            texes.push_back(tex);
        }
        
        nframes = texes.size();
        
        if (nframes > 0) {
            setup(texes[0]->getWidth(), texes[0]->getHeight());
            ofLogVerbose("ofxDDSPlayer") << "inited with " << nframes << " frames";
        }
    }
    
    void update(float frames) {
        playhead+= frames;
        if (playhead >= nframes) playhead-= nframes;
    }
    
    void draw() {
        

        int counterf = int(floor(playhead));
        int frames[2] = { counterf, counterf + 1 };
        float q = playhead - frames[0];
        
        if (frames[0] == nframes - 1) frames[1] = frames[0]; // stupid
        
        for (int i = 0; i < NFBOS; i++) {
            fbos[i].begin();
            texes[frames[i]]->draw();
            fbos[i].end();
        }
        
        ofPushMatrix();
        ofTranslate(this->width >> 1, this->height >> 1);
        
        mixShader.begin();
  
        mixShader.setUniformTexture("tex1", fbos[0].getTextureReference(), 0);
        mixShader.setUniformTexture("tex2", fbos[1].getTextureReference(), 1);
        
        mixShader.setUniform1f("q", q);
        
        mesh.draw();
        
        mixShader.end();
        ofPopMatrix();
        
//        ofSetColor(255);
//        ofDrawBitmapString(ofToString(q), 10, 10);
//        ofDrawBitmapString(ofToString(playhead), 10, 20);
//        ofDrawBitmapString(ofToString(frames[0]) + string(" : ") + ofToString(frames[1]), 10, 30);

    }
    
    int getTotalNumFrames() { return nframes; }
    float getWidth() { return width; }
    float getHeight() { return height; }
    float getPosition() { return playhead; }
    
};