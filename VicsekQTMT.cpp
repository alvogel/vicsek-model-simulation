#include <vector>
#include <math.h>
#include <complex>
#include <random>
#include <iostream>
#include <thread>
#include <mutex>

#include "VicsekQTMT.h"

std::mutex mu;

void VicsekQTMT::hightlightNeighbours(int x, int y)
{
    this->createQuadTree();

    std::vector<Particle*> neighbours;
    this->getNeighbours(x, y, neighbours);

    for(int n_np=0; n_np < neighbours.size(); n_np++)
    {
        neighbours[n_np]->highlighted = true;
    }

    this->qt1.clear();
    this->qt2.clear();
    this->qt3.clear();
    this->qt4.clear();
    this->qt5.clear();
    this->qt6.clear();
    this->qt7.clear();
    this->qt8.clear();
}

void VicsekQTMT::createQuadTree()
{
    for(int i=0; i<this->n; i++)
    {
        this->p[i].processed = false;
    }

    std::thread t11(&VicsekQTMT::Tree, this, std::ref(this->qt1));
    std::thread t22(&VicsekQTMT::Tree, this, std::ref(this->qt2));
    std::thread t33(&VicsekQTMT::Tree, this, std::ref(this->qt3));
    std::thread t44(&VicsekQTMT::Tree, this, std::ref(this->qt4));
    std::thread t55(&VicsekQTMT::Tree, this, std::ref(this->qt5));
    std::thread t66(&VicsekQTMT::Tree, this, std::ref(this->qt6));
    std::thread t77(&VicsekQTMT::Tree, this, std::ref(this->qt7));
    std::thread t88(&VicsekQTMT::Tree, this, std::ref(this->qt8));

    t11.join();
    t22.join();
    t33.join();
    t44.join();
    t55.join();
    t66.join();
    t77.join();
    t88.join();

}

void VicsekQTMT::Tree(QuadTree& quad)
{
    for(int i=0; i < this->n; i++)
    {
        if(!this->p[i].processed)
        {
            if(quad.insertPoint(&(this->p[i])))
            {
                this->p[i].processed = true;
            }
        }
    }
}

void VicsekQTMT::Chunk(unsigned int from, unsigned int to)
{

    float dia = 2 * this->radius;
    float square_r = this->radius*this->radius;
    float square_dia = dia*dia;

    //std::vector<Particle*> neighbour_points;
    //Rectangle query_r;

    for(int i=from; i<to; i++)
    {
        float sum_vx = 0;
        float sum_vy = 0;

        std::vector<Particle*> np;
        this->getNeighbours(this->p[i].x, this->p[i].y, np);

        for(int j=0; j < np.size(); j++)
        {
            std::complex<float> temp = std::polar((float)1.0, np[j]->dir);

            sum_vx += temp.real();
            sum_vy += temp.imag();

        }

        std::complex<float> temp_new_v (sum_vx, sum_vy);

        this->p[i].new_dir = std::arg(temp_new_v);

    }
}

void VicsekQTMT::getNeighbours(int x, int y, std::vector<Particle*> &np)
{
    if(x >= 0 && x <= this->w && y >= 0 && y <= this->h)
    {
        std::vector<Particle*> qt_np;

        float dia = 2 * this->radius;
        float square_r = this->radius * this->radius;
        float square_dia = dia * dia;

        Rectangle query_r;

        query_r.x = x;
        query_r.y = y;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt1.query(query_r, qt_np);
        this->qt2.query(query_r, qt_np);
        this->qt3.query(query_r, qt_np);
        this->qt4.query(query_r, qt_np);
        this->qt5.query(query_r, qt_np);
        this->qt6.query(query_r, qt_np);
        this->qt7.query(query_r, qt_np);
        this->qt8.query(query_r, qt_np);

        query_r.x = x + this->w;
        query_r.y = y;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt1.query(query_r, qt_np);
        this->qt2.query(query_r, qt_np);
        this->qt3.query(query_r, qt_np);
        this->qt4.query(query_r, qt_np);
        this->qt5.query(query_r, qt_np);
        this->qt6.query(query_r, qt_np);
        this->qt7.query(query_r, qt_np);
        this->qt8.query(query_r, qt_np);

        query_r.x = x - this->w;
        query_r.y = y;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt1.query(query_r, qt_np);
        this->qt2.query(query_r, qt_np);
        this->qt3.query(query_r, qt_np);
        this->qt4.query(query_r, qt_np);
        this->qt5.query(query_r, qt_np);
        this->qt6.query(query_r, qt_np);
        this->qt7.query(query_r, qt_np);
        this->qt8.query(query_r, qt_np);

        query_r.x = x;
        query_r.y = y + this->h;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt1.query(query_r, qt_np);
        this->qt2.query(query_r, qt_np);
        this->qt3.query(query_r, qt_np);
        this->qt4.query(query_r, qt_np);
        this->qt5.query(query_r, qt_np);
        this->qt6.query(query_r, qt_np);
        this->qt7.query(query_r, qt_np);
        this->qt8.query(query_r, qt_np);

        query_r.x = x;
        query_r.y = y - this->h;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt1.query(query_r, qt_np);
        this->qt2.query(query_r, qt_np);
        this->qt3.query(query_r, qt_np);
        this->qt4.query(query_r, qt_np);
        this->qt5.query(query_r, qt_np);
        this->qt6.query(query_r, qt_np);
        this->qt7.query(query_r, qt_np);
        this->qt8.query(query_r, qt_np);

        float sum_vx = 0;
        float sum_vy = 0;

        int size = qt_np.size();

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

            dx_c = x - qt_np[j]->x;
            dx_cs = dx_c * dx_c;
            dy_c = y - qt_np[j]->y;
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
                np.push_back(qt_np[j]);
            }
        }
    }
}

VicsekQTMT::Step()
{
    // create QuadTree
    this->createQuadTree();

    this->step_count++;

    int step = this->n/8;

    std::thread t1(&VicsekQTMT::Chunk, this, 0, step);
    std::thread t2(&VicsekQTMT::Chunk, this, step, 2*step);
    std::thread t3(&VicsekQTMT::Chunk, this, 2*step, 3*step);
    std::thread t4(&VicsekQTMT::Chunk, this, 3*step, 4*step);
    std::thread t5(&VicsekQTMT::Chunk, this, 4*step, 5*step);
    std::thread t6(&VicsekQTMT::Chunk, this, 5*step, 6*step);
    std::thread t7(&VicsekQTMT::Chunk, this, 6*step, 7*step);
    std::thread t8(&VicsekQTMT::Chunk, this, 7*step, 8*step);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();

    this->update_pos_vel();

    this->qt1.clear();
    this->qt2.clear();
    this->qt3.clear();
    this->qt4.clear();
    this->qt5.clear();
    this->qt6.clear();
    this->qt7.clear();
    this->qt8.clear();

}

// Draws the particles

VicsekQTMT::Draw(SDL_Renderer *r)
{
    Vicsek::Draw(r);
}
