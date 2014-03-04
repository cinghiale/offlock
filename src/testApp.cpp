#include "testApp.h"

//--------------------------------------------------------------
ofVec3f random_position() {
    return {
        ofRandom(-40, 40),
        ofRandom(-40, 40),
        ofRandom(-40, 40)
    };
}
ofVec3f random_velocity() {
    return {
        ofRandom(-5, 5),
        ofRandom(-5, 5),
        ofRandom(-5, 5)
    };
}
extern int boids_number;
void testApp::setup(){
    flocks[0] = new flock::Flock(boids_number);
    flocks[1] = new flock::Flock(boids_number);
    shader.load("f1");
    cpu_flock.bounding_cube_length = 500;

    {
        float s = cpu_flock.bounding_cube_length;
        ofVec3f const verts[] = {
            {+s, -s, +s},
            {+s, -s, -s},
            {-s, -s, -s},
            {-s, -s, +s},
            {+s, +s, +s},
            {+s, +s, -s},
            {-s, +s, -s},
            {-s, +s, +s}
        };
        ofIndexType const faces[] = {
            0, 1, 2,
            2, 3, 0,

            0, 1, 5,
            5, 4, 0,
            1, 2, 6,
            6, 5, 1,
            2, 3, 7,
            7, 6, 2,
            3, 0, 4,
            4, 7, 3,

            4, 5, 6,
            6, 7, 4
        };
        ofFloatColor const colors[] = {
            {0, 255, 0},
            {0, 255, 0},
            {0, 255, 0},
            {0, 255, 0},
            {0, 255, 0},
            {0, 255, 0},
            {0, 255, 0},
            {0, 255, 0}
        };

        cube.setVertexData(verts, 8, GL_STATIC_DRAW);
        cube.setColorData(colors, 8, GL_STATIC_DRAW);
        cube.setIndexData(faces, 24, GL_STATIC_DRAW);
    }

    current_flock = 0;
    next_flock = 1;

    flocks[current_flock]->add(random_position(), random_velocity());
    flocks[current_flock]->add(random_position(), random_velocity());

    {
        ofVec3f const verts[] = {
            {-2, 0, 0},
            {2, 0, -1},
            {2, 0, 1},
            {2, 2, 0}
        };
        ofIndexType const faces[] = {
            0, 1, 2,
            0, 1, 3,
            0, 3, 2,
            1, 2, 3
        };
        ofFloatColor const colors[] = {
            {255, 0, 0},
            {0, 255, 0},
            {0, 0, 255},
            {0, 255, 255}
        };
        vbo.setVertexData(verts, 4, GL_STATIC_DRAW);
        vbo.setColorData(colors, 4, GL_STATIC_DRAW);

        auto attribute = shader.getAttributeLocation("boid_position");
        this->vbo.setAttributeData(
            attribute,
            (float*)(flocks[current_flock]->positions.data()),
            3,
            flocks[current_flock]->max_size,
            GL_STREAM_DRAW);

        vbo.bind();
        glVertexAttribDivisor(attribute, 1);
        vbo.unbind();

        vbo.setIndexData(faces, 12, GL_STATIC_DRAW);
    }



    /*

    cl::Context ctx;
    cl::Program program;
    std::tie(ctx, program) = ocl::shared_cl_context({"./data/kernel1.cl"}, &cout);

    this->queue = ocl::create_queues(ctx)[0];
    this->shared_buffer = cl::BufferGL(ctx, CL_MEM_READ_WRITE, vbo.getVertId());
    this->shared_buffers.push_back(this->shared_buffer);

    vector<cl::Kernel> kernels;
    program.createKernels(&kernels);
    this->kernel = kernels[0];
    cout << "Kernel: " << this->kernel.getInfo<CL_KERNEL_FUNCTION_NAME>() << endl;
    this->kernel.setArg(0, this->shared_buffer);
    this->kernel.setArg(1, 9);
    */
}

//--------------------------------------------------------------
void testApp::update() {
    if(ofRandomuf() < 0.9 && !flocks[current_flock]->full()) {
        flocks[current_flock]->add(random_position(), random_velocity());
    }
    if (ofGetFrameNum() % 10 != 0) {
        //return;
    }
    cpu_flock.update(*flocks[current_flock], *flocks[next_flock]);
    std::swap(current_flock, next_flock);
    this->vbo.updateAttributeData(
        shader.getAttributeLocation("boid_position"),
        (float*)(flocks[current_flock]->positions.data()),
        flocks[current_flock]->size());
}

//--------------------------------------------------------------
void testApp::draw(){
    ofClear(0, 0, 0);
    shader.begin();
    shader.setUniform1i("cube_size", cpu_flock.bounding_cube_length);
    easyCam.begin();
        ofPushMatrix();
            cube.drawElements(GL_LINE_STRIP, 24);
            vbo.drawElementsInstanced(GL_TRIANGLES, 12, flocks[current_flock]->size());
        ofPopMatrix();
    easyCam.end();
    shader.end();
    /*
    ofCircle(300, 300, 30);
    try {
        this->queue.enqueueAcquireGLObjects(&this->shared_buffers);
        this->queue.enqueueNDRangeKernel(
            this->kernel,
            cl::NDRange(),
            cl::NDRange(1234944),
            cl::NDRange()
        );
        this->queue.enqueueReleaseGLObjects(&this->shared_buffers);
        this->queue.finish();
    }
    catch(cl::Error& e) {
        cout << e.err() << endl;
        throw;
    }
    //ofRotateZ(ofGetElapsedTimef());
    vbo.bind();
    vbo.draw(GL_TRIANGLES, 0, 3);
    shader.end();
    */
    string message = ofToString(ofGetFrameRate(), 2)+"fps;";
    message += " flock size: " + ofToString(flocks[0]->size());
    message += " (m)ass centre=" + ofToString(cpu_flock.coefficient_toward_centre_mass, 3);
    message += " (k)eep distance=" + ofToString(cpu_flock.coefficient_keep_distance, 3);
    message += " match (v)elocity=" + ofToString(cpu_flock.coefficient_match_velocity, 3);
    ofDrawBitmapString(message, 10, 15);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (!(key == 'm' || key == 'k' || key == 'v' || key == 'p' || key == 'q')) {
        return;
    }
    if (key == 'q') {
        cout << "bye bye" << endl;
        ofExit();
        return;
    }
    if (key == 'p') {
        cpu_flock.goal = ofVec3f(500, 0, 0);
        cpu_flock.coefficient_toward_goal = 0.01;
        return;
    }
    float values[4];
    float *target;

    if (key == 'm') {
        values[0] = 0.01;
        values[1] = 0.1;
        values[2] = 0;
        values[3] = -0.1;
        target = &cpu_flock.coefficient_toward_centre_mass;
    }
    else if (key == 'k') {
        values[0] = 0.4;
        values[1] = 0.8;
        values[2] = 1.4;
        values[3] = 0;
        target = &cpu_flock.coefficient_keep_distance;
    }
    else if (key == 'v') {
        values[0] = 0.125;
        values[1] = 0.250;
        values[2] = 0.500;
        values[3] = 0;
        target = &cpu_flock.coefficient_match_velocity;
    }
    auto count = sizeof(values)/sizeof(float);
    for (unsigned int ix=0; ix<count; ix++) {
        if (values[ix] == *target) {
            *target = values[(ix+1) % count];
            break;
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
