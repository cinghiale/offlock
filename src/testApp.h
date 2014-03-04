#pragma once

#define __CL_REFERENCE
#include "ofMain.h"
#include "flock.h"
#include <array>

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        flock::Flock* flocks[2];
        int current_flock;
        int next_flock;
        flock::algorithm::CPU cpu_flock;

		ofVbo vbo;
		ofVbo cube;

		ofShader shader;

		ofEasyCam easyCam;

        /*
        cl::CommandQueue queue;
        cl::BufferGL shared_buffer;
        cl::Kernel kernel;
        vector<cl::Memory> shared_buffers;
        */
};
