#include "flock.h"

namespace flock {

Flock::Flock(int max_size) : max_size(max_size), positions(max_size), velocities(max_size) {
}

bool Flock::full() const {
    return this->_size >= this->max_size;
}
int Flock::size() const {
    return this->_size;
}
void Flock::resize(int s) {
    if (s > this->max_size) {
        throw std::runtime_error("invalide size");
    }
    _size = s;
}

void Flock::add(ofVec3f const& position, ofVec3f const& velocity) {
    if(this->full()) {
        throw std::runtime_error("flock full");
    }
    auto ix = this->_size++;
    this->positions[ix] = position;
    this->velocities[ix] = velocity;
}

namespace algorithm {
void CPU::update(Flock const& src, Flock& dst) {
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
        velocity += (centre_mass - boid) * coefficient_toward_centre_mass;
        velocity += keep_distance * coefficient_keep_distance;
        velocity += (match_velocity - src.velocities[px]) * coefficient_match_velocity;
        velocity += (goal - boid) * coefficient_toward_goal;
        velocity += wind * coefficient_wind;

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

ofVec3f CPU::bound_cube(ofVec3f const& boid) {
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
}
}
