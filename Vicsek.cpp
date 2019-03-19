#include <random>
#include <vector>
#include <iostream>
#include <complex>
#include <mutex>

#include "SDL.h"
#include <ctime>
#include <chrono>

#include "Particle.h"
#include "Vicsek.h"

extern std::mutex mm;

auto seed_time = std::chrono::system_clock::now().time_since_epoch().count();

std::random_device rd{};
std::seed_seq seed{seed_time};
std::mt19937 gen{seed};

Vicsek::Vicsek() {}

Vicsek::Vicsek(unsigned short width, unsigned short height, float v, float radius, float eta, unsigned int n_particles)
{
    this->step_count = 0;
    this->w = width;
    this->h = height;
    this->radius = radius;
    this->eta = eta;
    this->v = v;
    this->n = n_particles;
    this->d = std::normal_distribution<float>(0, this->eta / 2.0);
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
        p[i].x = (float)d_x(gen);
        p[i].y = (float)d_y(gen);
        p[i].dir = (float)d_dir(gen);
    }
}

void Vicsek::reset()
{
    this->shuffle();
    this->step_count = 0;
}

void Vicsek::getNeighbours(int x, int y, std::vector<Particle*> &p)
{
    float dia = this->radius * 2;
    float square_r = this->radius * this->radius;
    float square_dia = dia * dia;

    for(int j=0; j < this->n; j++)
    {
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
            p.push_back(&this->p[j]);
        }
    }
}

Vicsek::Step()
{
    this->step_count++;

    float dia = this->radius * 2;
    float square_r = this->radius * this->radius;
    float square_dia = dia * dia;

    for(int i=0; i < this->n; i++)
    {
        float sum_vx = 0;
        float sum_vy = 0;

        for(int j=0; j < this->n; j++)
        {
            float distance_center = pow(this->p[i].x - this->p[j].x, 2) + pow(this->p[i].y - this->p[j].y, 2);
            float distance_east = pow(this->p[i].x - (this->p[j].x + this->w), 2) + pow(this->p[i].y - this->p[j].y, 2);
            float distance_north = pow(this->p[i].x - this->p[j].x, 2) + pow(this->p[i].y - (this->p[j].y - this->h), 2);
            float distance_west = pow(this->p[i].x - (this->p[j].x - this->w), 2) + pow(this->p[i].y - this->p[j].y, 2);
            float distance_south = pow(this->p[i].x - this->p[j].x, 2) + pow(this->p[i].y - (this->p[j].y + this->h), 2);

            if(     distance_center < square_r ||
                    distance_east < square_r ||
                    distance_north < square_r ||
                    distance_west < square_r ||
                    distance_south < square_r
              )

            {
                std::complex<float> temp = std::polar((float)1.0, this->p[j].dir);

                sum_vx += temp.real();
                sum_vy += temp.imag();

            }

        }

        std::complex<float> temp_new_v (sum_vx, sum_vy);

        this->p[i].new_dir = std::arg(temp_new_v);

    }

    this->update_pos_vel();
}

void Vicsek::update_pos_vel()
{
    for(int i=0; i<this->p.size(); i++)
    {
        float noise = this->d(gen);

        this->p[i].dir = this->p[i].new_dir + noise;

        std::complex<float> temp = std::polar((float)1.0, this->p[i].dir);

        this->p[i].x += this->v * temp.real();
        this->p[i].y += this->v * temp.imag();

        // reset color to white

        p[i].color_r = 255;
        p[i].color_g = 255;
        p[i].color_b = 255;

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

float Vicsek::calc_avg_norm_vel()
{
    float sum_vx = 0;
    float sum_vy = 0;

    for(int i=0; i<this->n; i++)
    {
        std::complex<float> temp = std::polar((float)1.0, this->p[i].dir);

        sum_vx += temp.real();
        sum_vy += temp.imag();
    }

    float length = sqrt(pow(sum_vx, 2) + pow(sum_vy, 2));

    float anv = length / this->n;

    return anv;
}

// Draws the particles

Vicsek::Draw(SDL_Renderer* r)
{
    for(int i=0; i < this->n; i++)
    {
        std::complex<float> temp = std::polar((float)1.0, this->p[i].dir);

        SDL_SetRenderDrawColor( r, p[i].color_r, p[i].color_g, p[i].color_b, 0x00 );
        //SDL_RenderDrawPoint(r, (int)p[i].x, (int)p[i].y);
        SDL_RenderDrawLine(r, p[i].x, p[i].y, p[i].x+(10*-temp.real()),p[i].y+(10*-temp.imag()));

    }
}


