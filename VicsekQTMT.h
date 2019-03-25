#ifndef VICSEKQTMT_H_INCLUDED
#define VICSEKQTMT_H_INCLUDED

#include "Vicsek.h"
#include "QuadTree.h"

class VicsekQTMT : public Vicsek
{
private:


public:

    //QuadTree qt;
    QuadTree qt1,qt2,qt3,qt4,qt5,qt6,qt7,qt8;

    VicsekQTMT();

    VicsekQTMT(SDL_Renderer* r, unsigned short width, unsigned short height, float v,  float radius, float eta, unsigned int n_particles) : Vicsek(r, width, height, v, radius, eta, n_particles)
    {
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
    }

    void createQuadTree();

    void Tree(QuadTree& quad);

    void Chunk(unsigned int from, unsigned int to);

    void hightlightNeighbours(int x, int y);

    void getNeighbours(int x, int y, std::vector<Particle*> &np);

    Step();

    Draw(SDL_Renderer* r);

};

#endif // VICSEKQTMT_H_INCLUDED
