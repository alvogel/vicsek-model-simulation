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

    file << "#"  << "\t" << "CPD"  << "\t" << "WIDTH"  << "\t" << "HEIGHT"  << "\t" << "density"  << "\t" << "#simulation_steps"  << "\t" << "particle_count" << "\t" << "average_normalized_velocity" << "\t" << "velocity" << "\t" << "eta" << "\t" << "radius" <<"\n";

    std::list<ResultEntry>::iterator iter_data;

    int count = 0;

    for(iter_data = this->data.begin(); iter_data != this->data.end(); iter_data++)
    {
        file    << count++ << "\t";
        file    << std::fixed << std::setprecision(7) << iter_data->cps << "\t";
        file    << std::fixed << std::setprecision(0) << iter_data->world_width << "\t";
        file    << std::fixed << std::setprecision(0) << iter_data->world_height << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->density << "\t";
        file    << iter_data->n_iterations << "\t";
        file    << iter_data->n_particles << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->avg_norm_vel << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->v << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->eta << "\t";
        file    << std::fixed << std::setprecision(7) << (float)iter_data->radius << "\n";
    }

    file.close();

}

void Result::addEntry(float cps, unsigned int w, unsigned int h, float rho, unsigned int n_i, unsigned int n_p, float anv, float v, float eta, float radius)
{

    ResultEntry d;
    d.cps = cps;
    d.world_width = w;
    d.world_height = h;
    d.density = rho;
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

    d.cps = 0;
    d.world_width = 0;
    d.world_height = 0;
    d.density = 0;
    d.n_iterations = 0;
    d.n_particles = 0;
    d.avg_norm_vel = 0;
    d.v = 0;
    d.eta = 0;
    d.radius = 0;

    std::list<ResultEntry>::iterator iter_data = this->data.end();

    if(last_n > this->data.size())
    {
        last_n = this->data.size();

    }

    for(int i=0; i<last_n; i++)
    {
        iter_data--;

        d.cps += iter_data->cps;
        d.world_width = iter_data->world_width;
        d.world_height = iter_data->world_height;
        d.density = iter_data->density;
        d.n_iterations = iter_data->n_iterations;
        d.n_particles = iter_data->n_particles;
        d.avg_norm_vel += iter_data->avg_norm_vel;
        d.v = iter_data->v;
        d.eta = iter_data->eta;
        d.radius = iter_data->radius;
    }

    /*d.n_iterations = iter_data->n_iterations;
    d.n_particles = iter_data->n_particles;*/
    d.cps /= last_n;
    d.avg_norm_vel /= last_n;

    /*d.v = iter_data->v;
    d.eta = iter_data->eta;
    d.radius = iter_data->radius;*/

    return d;
}

void Result::clear()
{
    this->data.clear();
}

