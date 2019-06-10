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

    float eta;
    std::normal_distribution<float> d{0,0};
    SDL_Surface* image;
    SDL_Texture *texture;

public:
    unsigned int step_count;
    unsigned short w;
    unsigned short h;
    float v;
    float radius;
    unsigned int n;
    std::vector<Particle> p;

    // Constructor
    Vicsek();

    // Constructor
    Vicsek(SDL_Renderer* r, unsigned short width, unsigned short height, float v, float radius, float eta, unsigned int n_particles);

    // run one Simulation step

    Step();

    // Draw Particles to renderer

    Draw(SDL_Renderer* r);

    // highlight neigbours of mouse position

    void highlightNeighbours(int x, int y);

    // get neighbours particles of position

    void getNeighbours(int x, int y, std::vector<Particle*> &p);

    // shuffle particles positions and directions

    void shuffle();

    // apply new positions and directions

    void update_pos_vel();

    float calc_avg_norm_vel();

    // set noise parameter

    void setEta(float eta);

    // set amount of particles

    void setParticleCount(unsigned int n);

    // get noise parameter

    float getEta();

    // reset simulation shuffle particles positions and directions

    void reset();

};


#endif // VICSEK_H_INCLUDED
