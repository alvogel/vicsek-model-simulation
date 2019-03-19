#include "Rectangle.h"

Rectangle::Rectangle() {}
Rectangle::Rectangle(float x, float y, float w, float h)
{
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
    this->half_width = w / 2;
    this->half_height = h / 2;
    this->top = y - this->half_height;
    this->bottom = y + this->half_height;
    this->left = x - this->half_width;
    this->right = x + this->half_width;
}
void Rectangle::setWidth(float w)
{
    this->width = w;
    this->half_width = w / 2;
    this->left = x - this->half_width;
    this->right = x + this->half_width;
}

float Rectangle::getWidth()
{
    return this->width;
}

void Rectangle::setHeight(float h)
{
    this->height = h;
    this->half_height = h / 2;
    this->top = y - this->half_height;
    this->bottom = y + this->half_height;
}

float Rectangle::getHeight()
{
    return this->height;
}

// checks if a point is inside a rectangle

bool Rectangle::contains(float x, float y)
{
    float left = this->x - (this->width / 2);
    float bottom = this->y - (this->height / 2);
    float right = this->x + (this->width / 2);
    float top = this->y + (this->height / 2);

    if(x >= left && x <= right && y >= bottom && y <= top)
        return true;
    else
        return false;
}

// checks if two rectangles are intersecting each other

bool Rectangle::intersects(Rectangle& range)
{
    if (range.right <= this->left)
        return false;
    else if (range.left >= this->right)
        return false;
    else if (range.bottom <= this->top)
        return false;
    else if (range.top >= this->bottom)
        return false;
    else
        return true;
}

