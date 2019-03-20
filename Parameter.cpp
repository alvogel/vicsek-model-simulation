#include "Parameter.h"
#include <iostream>

Parameter::Parameter()
{

}

Parameter::Parameter(float start, float end, int steps)
{
    this->start = start;
    this->end = end;
    this->steps = steps;
    if(this->steps > 0)
    {
        this->inc = (this->end - this->start) / this->steps;
    }
    else
    {
        this->inc = 0;
    }
    this->current_step = 0;

}

float Parameter::getStart()
{
    return this->start;
}

float Parameter::getCurrent()
{
    return this->start + (this->current_step * this->inc);
}

float Parameter::getEnd()
{
    return this->end;
}

int Parameter::getCurrentStep()
{
    return this->current_step;
}

void Parameter::next()
{
    if(!this->isFinished())
    {
        this->current_step++;
    }

}

void Parameter::reset()
{

    this->current_step = 0;

}

bool Parameter::isFinished()
{
    if(this->current_step <= this->steps)
        return false;
    else
        return true;
}
