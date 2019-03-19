#include <math.h>

#include "Particle.h"

Particle::Particle()
{

    this->color_r = 255;
    this->color_g = 255;
    this->color_b = 255;

}

Particle::Particle(float x, float y, float dir)
{
    this->x = x;
    this->y = y;
    this->dir = dir;
    this->processed = false;
    this->color_r = 255;
    this->color_g = 255;
    this->color_b = 255;
}
