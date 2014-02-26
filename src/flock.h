#pragma once
#include <array>
#include <stdexcept>
#include "ofVectorMath.h"

namespace flock {

using std::array;

template<size_t N>
class Flock {
    private:
        int _size{0};
    public:
        static int const max_size = N;

        array<ofVec3f, N> positions;
        array<ofVec3f, N> velocities;

        bool full() {
            return this->_size >= this->max_size;
        }

        int size() const {
            return this->_size;
        }

        void resize(int s) {
            if (s > this->max_size) {
                throw std::runtime_error("invalide size");
            }
            _size = s;
        }

        void add(ofVec3f const& position, ofVec3f const& velocity) {
            if(this->full()) {
                throw std::runtime_error("flock full");
            }
            auto ix = this->_size++;
            this->positions[ix] = position;
            this->velocities[ix] = velocity;
        }
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

        template<size_t N>
        void update(Flock<N> const& src, Flock<N>& dst) {
            for (int px=0; px<src.size(); px++) {
                auto& boid = src.positions[px];

                ofVec3f centre_mass;
                ofVec3f keep_distance;
                ofVec3f match_velocity;
                for (int ix=0; ix<src.size(); ix++) {
                    if (ix != px) {
                        // rule #1 - toward centre of mass
                        centre_mass += src.positions[ix];

                        // rule #2 - keep distance from neighbors
                        if ((src.positions[ix] - boid).lengthSquared() < 100) {
                            keep_distance = keep_distance - (src.positions[ix] - boid);
                        }

                        // rule #3 - match velocities
                        match_velocity += src.velocities[ix];
                    }
                }

                centre_mass /= src.size() - 1;
                match_velocity /= src.size() - 1;

                ofVec3f velocity;
                velocity += (centre_mass - boid) * this->coefficient_toward_centre_mass;
                velocity += keep_distance * this->coefficient_keep_distance;
                velocity += (match_velocity - src.velocities[px]) * this->coefficient_match_velocity;

                if (this->bounding_cube_length) {
                    velocity += this->bound_cube(boid) * this->coefficient_bounding_cube;
                }

                dst.velocities[px] = src.velocities[px] + velocity;
                if (dst.velocities[px].lengthSquared() > this->max_velocity*this->max_velocity) {
                    dst.velocities[px] = dst.velocities[px].normalize() * this->max_velocity;
                }
                dst.positions[px] = boid + dst.velocities[px];
            }
            dst.resize(src.size());
        }

        ofVec3f bound_cube(ofVec3f const& boid) {
            ofVec3f vel;

            auto dmin = -this->bounding_cube_length;
            auto dmax = this->bounding_cube_length;

            if (boid.x < dmin) {
                vel.x = 1;
            }
            else if (boid.x > dmax) {
                vel.x = -1;
            }
            if (boid.y < dmin) {
                vel.y = 1;
            }
            else if (boid.y > dmax) {
                vel.y = -1;
            }
            if (boid.z < dmin) {
                vel.z = 1;
            }
            else if (boid.z > dmax) {
                vel.z = -1;
            }
            return vel;
        }
};

}
}
