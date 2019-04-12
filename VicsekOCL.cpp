#include <vector>
#include <math.h>
#include <complex>
#include <random>
#include <iostream>
#include <chrono>

#include "VicsekOCL.h"

VicsekOCL::Step()
{
    this->step_count++;

    cl_int ret;

    std::complex<float> temp;

    for(int i=0; i<this->p.size(); i++)
    {
        //temp = std::polar((float)1.0, this->p[i].dir);

        matrix[i].x = this->p[i].x;
        matrix[i].y = this->p[i].y;
        matrix[i].vx = this->p[i].get_dir_x();//temp.real();
        matrix[i].vy = this->p[i].get_dir_y();//temp.imag();
    }

    header h;

    h.n = (unsigned int)this->n;
    h.r = (float)this->radius;
    h.width = (unsigned int)this->w;
    h.height = (unsigned int)this->h;

    ret = clEnqueueWriteBuffer(command_queue, this->header_mem_obj, CL_TRUE, 0, sizeof(header), (header*)&h, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, this->a_mem_obj, CL_TRUE, 0, sizeof(particle) * this->n, matrix, 0, NULL, NULL);

    this->cc = this->a_mem_obj;

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&header_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global, NULL, 0, NULL, NULL);

    clFinish(command_queue);

    ret = clEnqueueReadBuffer(command_queue, this->c_mem_obj, CL_TRUE, 0, this->n * sizeof(particle), this->C, 0, NULL, NULL);

    clFinish(command_queue);

    std::complex<float> temp_new_v;

    for(int i=0; i<this->p.size(); i++)
    {
        temp_new_v = std::complex<float>(this->C[i].vx, this->C[i].vy);

        this->p[i].new_dir = std::arg(temp_new_v);
    }

    this->update_pos_vel();


}
