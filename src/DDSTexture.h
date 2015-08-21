/**     ___           ___           ___                         ___           ___
 *     /__/\         /  /\         /  /\         _____         /  /\         /__/|
 *    |  |::\       /  /::\       /  /::|       /  /::\       /  /::\       |  |:|
 *    |  |:|:\     /  /:/\:\     /  /:/:|      /  /:/\:\     /  /:/\:\      |  |:|
 *  __|__|:|\:\   /  /:/~/::\   /  /:/|:|__   /  /:/~/::\   /  /:/  \:\   __|__|:|
 * /__/::::| \:\ /__/:/ /:/\:\ /__/:/ |:| /\ /__/:/ /:/\:| /__/:/ \__\:\ /__/::::\____
 * \  \:\~~\__\/ \  \:\/:/__\/ \__\/  |:|/:/ \  \:\/:/~/:/ \  \:\ /  /:/    ~\~~\::::/
 *  \  \:\        \  \::/          |  |:/:/   \  \::/ /:/   \  \:\  /:/      |~~|:|~~
 *   \  \:\        \  \:\          |  |::/     \  \:\/:/     \  \:\/:/       |  |:|
 *    \  \:\        \  \:\         |  |:/       \  \::/       \  \::/        |  |:|
 *     \__\/         \__\/         |__|/         \__\/         \__\/         |__|/
 *
 *  Description:
 *
 *  DDSTexture.h, created by Marek Bereza on 05/04/2013.
 */

#pragma once

#include "ofMain.h"
#include "nv_dds_mod.h"
#define OFFSET_U 8.f
#define OFFSET_V 2.f


class DDSTexture: public ofBaseDraws { // public ofHasTexture or something?
private:
	GLuint texobj;
	bool loaded;
	int width;
	int height;
	nv_dds::CDDSImage image;
	bool generatedTexture;
	float offsetU;
	float offsetV;

public:
	DDSTexture() {
	    static int count = 0;

	    count++;
	   // printf("DDSTexture::DDSTexture() allocation count %d\n", count);
		loaded = false;
		width = 0;
		height = 0;
		generatedTexture = false;
		offsetU = 0;
		offsetV = 0;
	}

	void loadData(string path) {
		loaded = false;
		image.load(ofToDataPath(path).c_str(), false);
	}
	void uploadTexture(int width, int height, unsigned char *data, int size) {
		if(!generatedTexture) {
			glGenTextures(1, &texobj);
			generatedTexture = true;
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texobj);

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
								  width, height, 0, size,
								  data);

		glBindTexture(GL_TEXTURE_2D, 0);
		this->width = width;
		this->height = height;
		offsetU = OFFSET_U/width;
		offsetV = OFFSET_V/height;
		loaded = true;
	}



	void uploadTexture() {
		if(!generatedTexture) {
			glGenTextures(1, &texobj);
			generatedTexture = true;
		}


		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texobj);

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, image.get_format(),
								  image.get_width(), image.get_height(), 0, image.get_size(),
								  image);

		for (int i = 0; i < image.get_num_mipmaps(); i++)
		{
			nv_dds::CSurface mipmap = image.get_mipmap(i);

			glCompressedTexImage2DARB(GL_TEXTURE_2D, i+1, image.get_format(),
									  mipmap.get_width(), mipmap.get_height(), 0, mipmap.get_size(),
									  mipmap);
		}
		this->width = image.get_width();
		this->height = image.get_height();
		offsetU = OFFSET_U/width;
		offsetV = OFFSET_V/height;
		loaded = true;
	}


	void load(string path) {


		loadData(path);

		uploadTexture();
        image.clear();
	}


	void draw() {
		draw(0,0);
	}


	void draw(float x, float y) {
		draw(x, y, getWidth(), getHeight());
	}

	void draw(float x, float y, float w, float h, float z) {
		if(!loaded) {
			//printf("Not loaded\n");
			//ofRect(x, y, w, h);
			return;
		}


		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, texobj );
		glBegin( GL_QUADS );
		glTexCoord2f(offsetU,offsetV);			glVertex3f(x,y,z);
		glTexCoord2f(1.0-offsetU,offsetV);		glVertex3f(x+w,y,z);
		glTexCoord2f(1.0-offsetU,1.0-offsetV);	glVertex3f(x+w,y+h,z);
		glTexCoord2f(offsetU,1.0-offsetV);		glVertex3f(x,y+h, z);
		glEnd();
		glBindTexture( GL_TEXTURE_2D, 0 );
	}


	void draw(float x, float y, float w, float h) {
		if(!loaded) {
			//printf("Not loaded\n");
			//ofRect(x, y, w, h);
			return;
		}
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, texobj );
		glBegin( GL_QUADS );
		glTexCoord2d(offsetU,offsetV);			glVertex2d(x,y);
		glTexCoord2d(1.0-offsetU,offsetV);		glVertex2d(x+w,y);
		glTexCoord2d(1.0-offsetU,1.0-offsetV);	glVertex2d(x+w,y+h);
		glTexCoord2d(offsetU,1.0-offsetV);		glVertex2d(x,y+h);
		glEnd();
		glBindTexture( GL_TEXTURE_2D, 0 );
	}


	float getHeight() {
		return height;
	}
	float getWidth() {
		return width;
	}
    
    GLuint getTextureObj() {
        return texobj;
    }

};
