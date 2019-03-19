#include <vector>
#include <math.h>
#include <complex>
#include <random>
#include <iostream>
#include <chrono>

#include "VicsekQT.h"

uint64_t time_ms1()
{
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return ms;
}

VicsekQT::Step()
{
    this->step_count++;

    int from = 0;
    int to = this->p.size();

    Rectangle r = Rectangle(this->w/2.0, this->h/2.0, this->w, this->h);

    this->qt = QuadTree(r,1,1);

    for(int i=0; i<to; i++)
    {
        qt.insertPoint(&this->p[i]);
    }

    Rectangle query_r;

    float dia = 2 * this->radius;
    float square_r = this->radius*this->radius;
    float square_dia = dia*dia;

    uint64_t start_chunk = time_ms1();
    int count = 0;

    for(int i=from; i<to; i++)
    {
        neighbour_points.clear();

        query_r.x = this->p[i].x;
        query_r.y = this->p[i].y;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        qt.query(query_r, neighbour_points);

        float sum_vx = 0;
        float sum_vy = 0;

        int size = neighbour_points.size();

        for(int j = 0; j<size; j++)
        {
            float distance = pow(this->p[i].x - neighbour_points[j]->x, 2) + pow(this->p[i].y - neighbour_points[j]->y, 2);

            if(distance < square_r)
            {
                std::complex<float> temp = std::polar((float)1.0, neighbour_points[j]->dir);

                sum_vx += temp.real();
                sum_vy += temp.imag();
            }
        }

        std::complex<float> temp_new_v (sum_vx, sum_vy);

        this->p[i].new_dir = std::arg(temp_new_v);

    }

    //std::cout << "Chunks: " <<count << " : "<< time_ms1() - start_chunk << std::endl;

    this->update_pos_vel();

    // Clear QuadTree
    this->qt.clear();
}

// Draws the particles

VicsekQT::Draw(SDL_Renderer *r)
{
    // Call super function
    Vicsek::Draw(r);
    //this->qt.Draw(r);

}
