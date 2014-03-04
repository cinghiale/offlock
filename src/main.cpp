#include "ofMain.h"
#include "testApp.h"
#include "ofGLProgrammableRenderer.h"
#include <cstdlib>

int boids_number = 0;
//========================================================================
int main(int argc, char* argv[]){
    if (argc > 1) {
        boids_number = atoi(argv[1]);
    }
    if (!boids_number) {
        boids_number = 100;
    }
    cout << "boids: " << boids_number << endl;
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    ofSetupOpenGL(1024,768, OF_WINDOW);         // <-------- setup the GL context

    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp( new testApp());

}
