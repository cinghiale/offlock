#pragma once

#define __CL_REFERENCE
#include "ofMain.h"
#include "ocl.h"
#include <array>

template<int N>
struct Boids {
    static int const max_size = N;
    int size{0};
    std::array<ofVec3f, N> positions;
    std::array<ofVec3f, N> velocities;
};

class CPUFlock {
    public:
        int bound_cube_size{0};

        template<size_t N>
        void update(std::array<ofVec3f, N>& positions, std::array<ofVec3f, N>& velocities) {
            for (int px=0; px<positions.size(); px++) {
                ofVec3f velocity;
                velocity += this->toward_center_mass(px, positions) / 100;
                velocity += this->keep_distance(px, positions);
                velocity += this->match_near(px, velocities) / 8;
                if(this->bound_cube_size) {
                    velocity += this->bound_cube(px, this->bound_cube_size, positions) * 10;
                }

                velocities[px] += velocity;
                this->limit_velocity(velocities[px]);
                positions[px] += velocities[px];
            }
        }

        void limit_velocity(ofVec3f& velocity) {
            int const limit = 10;
            if (velocity.lengthSquared() > limit*limit) {
                velocity = velocity / velocity.length() * limit;
            }
        }

        template<size_t N>
        ofVec3f toward_center_mass(int boidIx, std::array<ofVec3f, N> const& positions) {
            auto& boid = positions[boidIx];
            ofVec3f centre;
            for (int ix=0; ix<positions.size(); ix++) {
                if (ix == boidIx) {
                    continue;
                }
                centre += positions[ix];
            }
            centre /= positions.size() - 1;
            return centre - boid;
        }

        template<size_t N>
        ofVec3f keep_distance(int boidIx, std::array<ofVec3f, N> const& positions) {
            auto& boid = positions[boidIx];
            ofVec3f vel;
            for (int ix=0; ix<positions.size(); ix++) {
                if (ix == boidIx) {
                    continue;
                }
                if (positions[ix].squareDistance(boid) < 100) {
                    vel = vel - (positions[ix] - boid);
                }
            }
            return vel;
        }

        template<size_t N>
        ofVec3f match_near(int boidIx, std::array<ofVec3f, N> const& velocities) {
            auto& boid = velocities[boidIx];
            ofVec3f vel;
            for (int ix=0; ix<velocities.size(); ix++) {
                if (ix == boidIx) {
                    continue;
                }
                vel += velocities[ix];
            }
            vel /= velocities.size() - 1;
            return vel - boid;
        }

        template<size_t N>
        ofVec3f bound_cube(int boid, int bound_size, std::array<ofVec3f, N> const& positions) {
            int xmin = -bound_size;
            int ymin = -bound_size;
            int zmin = -bound_size;

            int xmax = bound_size;
            int ymax = bound_size;
            int zmax = bound_size;

            ofVec3f vel;
            auto& position = positions[boid];

            if (position.x < xmin) {
                vel.x = 1;
            }
            else if (position.x > xmax) {
                vel.x = -1;
            }
            if (position.y < ymin) {
                vel.y = 1;
            }
            else if (position.y > ymax) {
                vel.y = -1;
            }
            if (position.z < zmin) {
                vel.z = 1;
            }
            else if (position.z > zmax) {
                vel.z = -1;
            }
            return vel;
        }

};

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

		Boids<800> boids;
		ofVbo vbo;
		ofVbo cube;

		ofShader shader;
		CPUFlock flock;

		ofEasyCam easyCam;

        cl::CommandQueue queue;
        cl::BufferGL shared_buffer;
        cl::Kernel kernel;
        vector<cl::Memory> shared_buffers;
};
