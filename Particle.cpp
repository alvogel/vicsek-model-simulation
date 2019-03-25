#include <math.h>

#include "Particle.h"

Particle::Particle()
{

}

Particle::Particle(float x, float y, float dir)
{
    this->x = x;
    this->y = y;
    this->dir = dir;
    this->processed = false;
    this->highlighted = false;
}
