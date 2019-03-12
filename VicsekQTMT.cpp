#include <vector>
#include <math.h>
#include <complex>
#include <random>
#include <iostream>
#include <thread>
#include <mutex>

#include "VicsekQTMT.h"

std::mutex mu;

uint64_t time_ms()
{
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return ms;
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

    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<> d{0,this->eta/2};

    float dia = 2 * this->radius;
    float square_r = this->radius*this->radius;
    float square_dia = dia*dia;

    std::vector<Particle*> neighbour_points;
    neighbour_points.reserve(300);
    Rectangle query_r;

    for(int i=from; i<to; i++)
    {

        neighbour_points.clear();

        query_r.x = this->p[i].x;
        query_r.y = this->p[i].y;
        query_r.setWidth(dia);
        query_r.setHeight(dia);

        this->qt1.query(query_r, neighbour_points);
        this->qt2.query(query_r, neighbour_points);
        this->qt3.query(query_r, neighbour_points);
        this->qt4.query(query_r, neighbour_points);
        this->qt5.query(query_r, neighbour_points);
        this->qt6.query(query_r, neighbour_points);
        this->qt7.query(query_r, neighbour_points);
        this->qt8.query(query_r, neighbour_points);

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

}

VicsekQTMT::Step()
{
    int step = this->n/8;

    Rectangle r = Rectangle(this->w/2, this->h/2, this->w, this->h);

    int cap = 4;
    int mul = 1;

    Rectangle r1 = Rectangle(64, 128, 128, 256);
    this->qt1 = QuadTree(r1,cap,mul);
    Rectangle r2 = Rectangle(192, 128, 128, 256);
    this->qt2 = QuadTree(r2,cap,mul);
    Rectangle r3 = Rectangle(320, 128, 128, 256);
    this->qt3 = QuadTree(r3,cap,mul);
    Rectangle r4 = Rectangle(448, 128, 128, 256);
    this->qt4 = QuadTree(r4,cap,mul);
    Rectangle r5 = Rectangle(64, 384, 128, 256);
    this->qt5 = QuadTree(r5,cap,mul);
    Rectangle r6 = Rectangle(192, 384, 128, 256);
    this->qt6 = QuadTree(r6,cap,mul);
    Rectangle r7 = Rectangle(320, 384, 128, 256);
    this->qt7 = QuadTree(r7,cap,mul);
    Rectangle r8 = Rectangle(448, 384, 128, 256);
    this->qt8 = QuadTree(r8,cap,mul);

    uint64_t start_tree = time_ms();

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

    //std::cout << "Quadsize: " << this->qt.size() << std::endl;

    //std::cout << "Baumerstellung: " << time_ms() - start_tree << std::endl;

    uint64_t start_chunk = time_ms();

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


    //std::cout << "Chunks: " << time_ms() - start_chunk << std::endl;

    uint64_t start_pos = time_ms();

    this->update_pos_vel();

    //std::cout << "Neue Pos.: " << time_ms() - start_pos << std::endl;

    this->qt1.clear();
    this->qt2.clear();
    this->qt3.clear();
    this->qt4.clear();
    this->qt5.clear();
    this->qt6.clear();
    this->qt7.clear();
    this->qt8.clear();


}

VicsekQTMT::Draw(SDL_Renderer *r)
{
    //m->lock();
    Vicsek::Draw(r);
    //this->qt.Draw(r);
    /*this->qt1.Draw(r);
    this->qt2.Draw(r);
    this->qt3.Draw(r);
    this->qt4.Draw(r);
    this->qt5.Draw(r);
    this->qt6.Draw(r);
    this->qt7.Draw(r);
    this->qt8.Draw(r);*/
}
