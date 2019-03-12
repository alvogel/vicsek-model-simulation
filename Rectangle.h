#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include "Rectangle.h"

class Rectangle
{
private:


public:
    short width;
    short height;
    short half_width;
    short half_height;
    short x;
    short y;
    short top;
    short bottom;
    short left;
    short right;


    Rectangle();

    Rectangle(float x, float y, float w, float h);

    void setWidth(float w);

    float getWidth();

    void setHeight(float h);

    float getHeight();

    bool contains(float x, float y);

    bool intersects(Rectangle& range);
};


#endif // RECTANGLE_H_INCLUDED
