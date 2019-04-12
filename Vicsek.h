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

    Step();

    Draw(SDL_Renderer* r);

    void highlightNeighbours(int x, int y);

    void getNeighbours(int x, int y, std::vector<Particle*> &p);

    void shuffle();

    void update_pos_vel();

    float calc_avg_norm_vel();

    void setEta(float eta);

    void setParticleCount(unsigned int n);

    float getEta();

    void reset();

};


#endif // VICSEK_H_INCLUDED
