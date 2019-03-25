#include <vector>
#include <math.h>
#include <complex>
#include <random>
#include <iostream>
#include <chrono>

#include "VicsekQT.h"

void VicsekQT::hightlightNeighbours(int x, int y)
{
    this->createQuadTree();

    std::vector<Particle*> neighbours;
    this->getNeighbours(x, y, neighbours);

    for(int n_np=0; n_np < neighbours.size(); n_np++)
    {
        neighbours[n_np]->highlighted = true;
    }

    this->qt.clear();
}

void VicsekQT::createQuadTree()
{
    Rectangle r = Rectangle(this->w/2.0, this->h/2.0, this->w, this->h);

    this->qt = QuadTree(r,4,1);

    for(int i = 0; i < this->n; i++)
    {
        this->qt.insertPoint(&this->p[i]);
    }
}

void VicsekQT::getNeighbours(int x, int y, std::vector<Particle*> &np)
{
    if(x >= 0 && x <= this->w && y >= 0 && y <= this->h)
    {
        this->qt_np.clear();

        float dia = 2 * this->radius;
        float square_r = this->radius * this->radius;
        float square_dia = dia * dia;

        Rectangle query_r;

        query_r.x = x;
        query_r.y = y;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt.query(query_r, this->qt_np);

        query_r.x = x + this->w;
        query_r.y = y;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt.query(query_r, this->qt_np);

        query_r.x = x - this->w;
        query_r.y = y;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt.query(query_r, this->qt_np);

        query_r.x = x;
        query_r.y = y + this->h;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt.query(query_r, this->qt_np);

        query_r.x = x;
        query_r.y = y - this->h;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt.query(query_r, this->qt_np);

        float sum_vx = 0;
        float sum_vy = 0;

        int size = this->qt_np.size();

        for(int j = 0; j < size; j++)
        {
            float distance_center;
            float distance_east;
            float distance_north;
            float distance_west;
            float distance_south;

            float dx_c,dy_c;
            float dx_cs,dy_cs;
            float dx_e;
            float dy_n;
            float dx_w;
            float dy_s;

            dx_c = x - this->qt_np[j]->x;
            dx_cs = dx_c * dx_c;
            dy_c = y - this->qt_np[j]->y;
            dy_cs = dy_c * dy_c;
            distance_center = dx_cs + dy_cs;

            dx_e = dx_c + this->w;
            distance_east   = (dx_e * dx_e) + dy_cs;

            dy_n = dy_c - this->h;
            distance_north  = dx_cs + (dy_n * dy_n);

            dx_w = dx_c - this->w;
            distance_west   = (dx_w * dx_w) + dy_cs;

            dy_s = dy_c + this->h;
            distance_south  = dx_cs + (dy_s * dy_s);


            if(     distance_center < square_r ||
                    distance_east < square_r ||
                    distance_north < square_r ||
                    distance_west < square_r ||
                    distance_south < square_r
              )
            {
                this->np.push_back(this->qt_np[j]);
            }
        }
    }
}

VicsekQT::Step()
{
    // create QuadTree
    this->createQuadTree();

    this->step_count++;

    for(int i=0; i < this->n; i++)
    {
        float sum_vx = 0;
        float sum_vy = 0;

        //std::vector<Particle*> np;
        this->np.clear();
        this->getNeighbours(this->p[i].x, this->p[i].y, this->np);

        for(int j=0; j < np.size(); j++)
        {
            std::complex<float> temp = std::polar((float)1.0, this->np[j]->dir);

            sum_vx += temp.real();
            sum_vy += temp.imag();

        }

        std::complex<float> temp_new_v (sum_vx, sum_vy);

        this->p[i].new_dir = std::arg(temp_new_v);

    }

    this->update_pos_vel();

    this->qt.clear();

}


// Draws the particles

VicsekQT::Draw(SDL_Renderer *r)
{
    // Call super function
    Vicsek::Draw(r);
    //this->qt.Draw(r);

}
