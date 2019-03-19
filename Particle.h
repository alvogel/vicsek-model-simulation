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
    unsigned char color_r;
    unsigned char color_g;
    unsigned char color_b;

    Particle();
    Particle(float x, float y, float dir);
};

#endif // PARTICLE_H_INCLUDED
