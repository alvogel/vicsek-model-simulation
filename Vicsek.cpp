#include <random>
#include <vector>
#include <iostream>
#include <complex>

#include "SDL.h"

#include "Particle.h"
#include "Vicsek.h"

Vicsek::Vicsek() {}

Vicsek::Vicsek(unsigned short width, unsigned short height, float v, float radius, unsigned int n_particles)
{
    this->w = width;
    this->h = height;
    this->radius = radius;
    this->eta = 10;
    this->v = v;
    this->n = n_particles;

    for(int i=0; i<n_particles; i++)
    {
        p.push_back(Particle((float)(rand()%this->w), (float)(rand()%this->h), (rand()%628)/100));

    }
}

Vicsek::Step()
{
    float dia = this->radius*2;
    float square_r = this->radius*this->radius;
    float square_dia = dia*dia;

    for(int i=0; i < this->n; i++)
    {
        float sum_vx = 0;
        float sum_vy = 0;

        for(int j=0; j < this->n; j++)
        {
            float distance = pow(this->p[i].x - this->p[j].x, 2) + pow(this->p[i].y - this->p[j].y, 2);

            if(distance < square_r)
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

        this->p[i].dir = this->p[i].new_dir;

        std::complex<float> temp = std::polar((float)1.0, this->p[i].dir);

        this->p[i].x += this->v * temp.real();
        this->p[i].y += this->v * temp.imag();

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

Vicsek::Draw(SDL_Renderer* r)
{
    SDL_SetRenderDrawColor( r, 0xFF, 0xFF, 0xFF, 0x00 );

    for(int i=0; i < this->n; i++)
    {
        std::complex<float> temp = std::polar((float)1.0, this->p[i].dir);

        //SDL_RenderDrawPoint(r, (int)p[i].x, (int)p[i].y);
        SDL_RenderDrawLine(r, p[i].x, p[i].y, p[i].x+(10*-temp.real()),p[i].y+(10*-temp.imag()));

    }
}


