#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED

class Particle
{

private:

public:
    float x;
    float y;
    float dir;
    float new_dir;
    bool processed;
    bool highlighted;

    Particle();
    Particle(float x, float y, float dir);
};

#endif // PARTICLE_H_INCLUDED
