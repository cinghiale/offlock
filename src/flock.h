#pragma once
#include <vector>
#include <stdexcept>
#include "ofVectorMath.h"

namespace flock {

using std::vector;

class Flock {
    private:
        int _size{0};
    public:
        int const max_size;

        vector<ofVec3f> positions;
        vector<ofVec3f> velocities;

        Flock(int max_size);
        bool full() const;
        int size() const;
        void resize(int s);
        void add(ofVec3f const& position, ofVec3f const& velocity);
};

namespace algorithm {
class CPU {
    public:
        float coefficient_toward_centre_mass = 0.01;
        float coefficient_keep_distance = 0.4;
        float coefficient_match_velocity = 0.125;
        float max_velocity = 10;
        float bounding_cube_length = 0;
        float coefficient_bounding_cube = 10;
        ofVec3f goal;
        float coefficient_toward_goal = 0;

        void update(Flock const& src, Flock& dst);
        ofVec3f bound_cube(ofVec3f const& boid);
};

}
}
