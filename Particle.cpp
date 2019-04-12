#include <math.h>

#include "Particle.h"

// Particle constructor

Particle::Particle()
{

}

// Particle constructor

Particle::Particle(float x, float y, float dir)
{
    this->x = x;
    this->y = y;
    this->dir = dir;
    this->dir_x = cos(this->dir);
    this->dir_y = sin(this->dir);
    this->processed = false;
    this->highlighted = false;
}

// returns direction of particle in radiant

float Particle::get_dir()
{
    return this->dir;
}

// sets direction of particle in radiant

void Particle::set_dir(float d)
{
    this->dir = d;
    this->dir_x = cos(this->dir);
    this->dir_y = sin(this->dir);
}

// returns x component of velocity

float Particle::get_dir_x()
{
    return this->dir_x;
}

// returns y component of velocity

float Particle::get_dir_y()
{
    return this->dir_y;
}

