#include <iostream>
#include <iomanip>
#include <math.h>

#include "Result.h"
#include "ResultEntry.h"

Result::~Result()
{

}

Result::Result()
{

}

void Result::saveToFile()
{
    std::ofstream file;

    file.open("result.dat");

    file << "#simulation steps"  << "\t" << "particle count" << "\t" << "average normalized velocity" << "\t" << "velocity" << "\t" << "eta" << "\t" << "radius" <<"\n";

    std::list<ResultEntry>::iterator iter_data;

    for(iter_data = this->data.begin(); iter_data != this->data.end(); iter_data++)
    {
        file    << iter_data->n_iterations << "\t";
        file    << iter_data->n_particles << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->avg_norm_vel << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->v << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->eta << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->radius << "\n";
    }

    file.close();

}

void Result::addEntry(unsigned int n_i, unsigned int n_p, float anv, float v, float eta, float radius)
{

    ResultEntry d;
    d.n_iterations = n_i;
    d.n_particles = n_p;
    d.avg_norm_vel = anv;
    d.v = v;
    d.eta = eta;
    d.radius = radius;

    this->data.push_back(d);

}

ResultEntry Result::getAvgLastN(int last_n)
{
    ResultEntry d;

    d.n_iterations = 0;
    d.n_particles = 0;
    d.avg_norm_vel = 0;
    d.v = 0;
    d.eta = 0;
    d.radius = 0;

    std::list<ResultEntry>::iterator iter_data = this->data.end();

    for(int i=0; i<last_n; i++)
    {
        d.n_iterations = iter_data->n_iterations;
        d.n_particles = iter_data->n_particles;
        d.avg_norm_vel += iter_data->avg_norm_vel;
        d.v = iter_data->v;
        d.eta = iter_data->eta;
        d.radius = iter_data->radius;

        iter_data--;

    }

    /*d.n_iterations = iter_data->n_iterations;
    d.n_particles = iter_data->n_particles;*/
    d.avg_norm_vel /= last_n;

    //std::cout << "Durchscnitt: "<< d.avg_norm_vel << std::endl;

    /*d.v = iter_data->v;
    d.eta = iter_data->eta;
    d.radius = iter_data->radius;*/

    return d;
}

void Result::clear()
{
    this->data.clear();
}

