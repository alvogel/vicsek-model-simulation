#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED

class Particle
{

private:
    float dir;
    float dir_x;
    float dir_y;

public:
    float x;
    float y;
    float new_dir;
    bool processed;
    bool highlighted;


    // Particle constructor
    Particle();

    // Particle constructor
    Particle(float x, float y, float dir);

    // returns direction of particle in radiant
    float get_dir();

    // sets direction of particle in radiant
    void set_dir(float d);

    // returns x component of velocity
    float get_dir_x();

    // returns y component of velocity
    float get_dir_y();
};

#endif // PARTICLE_H_INCLUDED
