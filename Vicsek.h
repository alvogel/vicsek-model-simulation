#ifndef VICSEK_H_INCLUDED
#define VICSEK_H_INCLUDED

#include "Particle.h"
#include "SDL.h"
#include <vector>
#include <math.h>
#include <random>

extern std::random_device rd;
extern std::mt19937 gen;

class Vicsek
{
private:

    std::normal_distribution<float> d{0,0};

public:
    unsigned short w;
    unsigned short h;
    float eta;
    float v;
    float radius;
    unsigned int n;
    std::vector<Particle> p;

    Vicsek();

    Vicsek(unsigned short width, unsigned short height, float v, float radius, float eta, unsigned int n_particles);

    Step();

    Draw(SDL_Renderer* r);

    void update_pos_vel();

    float calc_avg_norm_vel();

};


#endif // VICSEK_H_INCLUDED
