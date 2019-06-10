#include <random>
#include <vector>
#include <iostream>
#include <complex>
#include <mutex>
#include <math.h>

#include "SDL.h"
#include <SDL_image.h>
#include <ctime>
#include <chrono>

#include "Particle.h"
#include "Vicsek.h"
#include "functions.h"

extern std::mutex mm;

auto seed_time = std::chrono::system_clock::now().time_since_epoch().count();

std::default_random_engine generator;
std::random_device rd{};
std::seed_seq seed{seed_time};
std::mt19937 gen{seed};

Vicsek::Vicsek() {}

Vicsek::Vicsek(SDL_Renderer* r, unsigned short width, unsigned short height, float v, float radius, float eta, unsigned int n_particles)
{
    this->image = IMG_Load("images/arrow.png");
    SDL_SetColorKey( this->image, SDL_TRUE, SDL_MapRGBA( this->image->format, 0x00, 0x00, 0x00, 0xFF ) );
    this->texture = SDL_CreateTextureFromSurface(r, image);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(this->texture, 128);

    this->step_count = 0;
    this->w = width;
    this->h = height;
    this->radius = radius;
    this->eta = eta;
    this->v = v;
    this->n = n_particles;
    this->p = std::vector<Particle>(n_particles);

    this->shuffle();
}

void Vicsek::shuffle()
{
    std::uniform_real_distribution<double> d_x(0, this->w);
    std::uniform_real_distribution<double> d_y(0, this->h);
    std::uniform_real_distribution<double> d_dir(0, 2 * M_PI);

    for(int i=0; i<this->p.size(); i++)
    {
        this->p[i].x = (float)d_x(gen);
        this->p[i].y = (float)d_y(gen);
        this->p[i].set_dir((float)d_dir(gen));
        //this->p[i].dir = (float)d_dir(gen);
        this->p[i].processed = false;
    }
}

// resets the simulation and shuffles particles directions and positions

void Vicsek::reset()
{
    this->shuffle();
    this->step_count = 0;
}

// Highlight all neighbour particles within interaction radius from mouse position

void Vicsek::highlightNeighbours(int x, int y)
{
    std::vector<Particle*> neighbours;

    this->getNeighbours(x, y, neighbours);

    // set all neighbours as highlighted

    for(int n_np=0; n_np < neighbours.size(); n_np++)
    {
        neighbours[n_np]->highlighted = true;
    }
}

// Find all neighbour particles within interaction radius

void Vicsek::getNeighbours(int x, int y, std::vector<Particle*> &p)
{
    float dia = this->radius * 2;
    float square_r = this->radius * this->radius;
    float square_dia = dia * dia;

    if(x >= 0 && x <= this->w && y >= 0 && y <= this->h)
    {
        for(int j=0; j < this->n; j++)
        {
            // calculate square distances(this saves time because we dont need to calculate the squareroot) for center map and shadow copies
            float distance_center   = pow(x - this->p[j].x, 2)              + pow(y - this->p[j].y, 2);
            float distance_east     = pow(x - (this->p[j].x + this->w), 2)  + pow(y - this->p[j].y, 2);
            float distance_north    = pow(x - this->p[j].x, 2)              + pow(y - (this->p[j].y - this->h), 2);
            float distance_west     = pow(x - (this->p[j].x - this->w), 2)  + pow(y - this->p[j].y, 2);
            float distance_south    = pow(x - this->p[j].x, 2)              + pow(y - (this->p[j].y + this->h), 2);

            if(     distance_center < square_r ||
                    distance_east < square_r ||
                    distance_north < square_r ||
                    distance_west < square_r ||
                    distance_south < square_r
              )
            {
                // append neighbour reference to neighbour list
                p.push_back(&this->p[j]);
            }
        }
    }
}

// Calculate one simualtion step

Vicsek::Step()
{
    this->step_count++;

    for(int i=0; i < this->n; i++)
    {
        float sum_vx = 0;
        float sum_vy = 0;

        // Find neighbours within interaction radius
        std::vector<Particle*> np;
        this->getNeighbours(this->p[i].x, this->p[i].y, np);

        // add vector components to sum
        for(int j=0; j < np.size(); j++)
        {
            sum_vx += np[j]->get_dir_x();
            sum_vy += np[j]->get_dir_y();
        }

        // put x and y into complex number
        std::complex<float> temp_new_v (sum_vx, sum_vy);

        // get angle from vector
        this->p[i].new_dir = std::arg(temp_new_v);

    }

    this->update_pos_vel();
}

// Updates new positions and directions to particles

void Vicsek::update_pos_vel()
{
    float noise;
    std::complex<float> temp;

    std::uniform_real_distribution<double> nse(-this->eta, this->eta);

    float two_pi = 2 * M_PI;

    for(int i=0; i<this->p.size(); i++)
    {
        // apply new position to particle
        this->p[i].x += this->v * this->p[i].get_dir_x();
        this->p[i].y += this->v * this->p[i].get_dir_y();

        // genrate noise
        noise = (double)nse(generator) * M_PI;

        // set direction for new particle + noise
        this->p[i].set_dir(this->p[i].new_dir + noise);

        // reset highlight
        p[i].highlighted = false;

        // crop direction that its 0 <= dir <= 2*pi
        if(this->p[i].get_dir() > two_pi)
        {
            this->p[i].set_dir(this->p[i].get_dir() - two_pi);
        }

        else if(this->p[i].get_dir() < 0)
        {
            this->p[i].set_dir(this->p[i].get_dir() + two_pi);
        }

        // crop position that its 0 <= x/y <= map size
        if(this->p[i].x > this->w)
        {
            this->p[i].x -= this->w;
        }

        else if(this->p[i].x < 0)
        {
            this->p[i].x += this->w;
        }

        else if(this->p[i].y > this->h)
        {
            this->p[i].y -= this->h;
        }

        else if(this->p[i].y < 0)
        {
            this->p[i].y += this->h;
        }
    }
}

// calculates and returns average normalized velocity

float Vicsek::calc_avg_norm_vel()
{
    float sum_vx = 0;
    float sum_vy = 0;

    for(int i=0; i<this->n; i++)
    {
        sum_vx += this->p[i].get_dir_x();
        sum_vy += this->p[i].get_dir_y();
    }

    float length = sqrt(pow(sum_vx, 2) + pow(sum_vy, 2));

    float anv = length / this->n;

    return anv;
}

// set noise parameter

void Vicsek::setEta(float eta)
{
    this->eta = eta;
    std::uniform_real_distribution<double> nse(0, this->eta);
}

// get noise parameter

float Vicsek::getEta()
{
    return this->eta;
}

// set amount of particles

void Vicsek::setParticleCount(unsigned int n)
{
    this->p.clear();
    this->n = n;
    this->p = std::vector<Particle>(n);

    this->shuffle();

}

// Draws the particles

Vicsek::Draw(SDL_Renderer* rr)
{
    SDL_Rect dstrect;
    SDL_Point center;
    SDL_Rect Rect = {0, 0, 0, 0};

    SDL_QueryTexture(this->texture, NULL, NULL, &Rect.w, &Rect.h);

    center.x = Rect.w/2;
    center.y = Rect.h/2;

    for(int i=0; i < this->n; i++)
    {
        dstrect.x = (int)p[i].x - Rect.w;
        dstrect.y = (int)p[i].y - Rect.h/2;
        dstrect.w = Rect.w;
        dstrect.h = Rect.h;

        unsigned short r,g,b;
        HSV_TO_RGB((this->p[i].get_dir() / (2*M_PI)) * 360.0, 1.0, 1.0, r, g, b);

        if(this->p[i].highlighted)
        {
            SDL_SetTextureColorMod(this->texture, 255, 255, 255);
        }

        else

        {
            SDL_SetTextureColorMod(this->texture, r, g, b);
        }

        SDL_RenderCopyEx(rr, this->texture, &Rect, &dstrect, (this->p[i].get_dir() / (2*M_PI)) * 360, &center, SDL_FLIP_NONE);

    }
}


