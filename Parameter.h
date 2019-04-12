#ifndef PARAMETER_H_INCLUDED
#define PARAMETER_H_INCLUDED

class Parameter
{

private:
    float start;
    float end;
    float inc;
    unsigned int steps;
    unsigned int current_step;

public:

    // Constructor
    Parameter();

    // Constructor
    Parameter(float start, float end, int steps);

    float getStart();
    float getCurrent();
    float getEnd();

    int getCurrentStep();

    void reset();

    void next();

    bool isFinished();
};

#endif // PARAMETER_H_INCLUDED
