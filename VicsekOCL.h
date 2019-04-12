#ifndef VICSEKOCL_H_INCLUDED
#define VICSEKOCL_H_INCLUDED

#include <complex>

#include "Vicsek.h"
#include "QuadTree.h"

#include "SDL.h"
#include <SDL_image.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

typedef struct
{
    unsigned int n;
    float r;
    unsigned int width;
    unsigned int height;

} header;

typedef struct
{
    float x;
    float y;
    float vx;
    float vy;
} particle;

class VicsekOCL : public Vicsek
{
private:

    particle *matrix = NULL;
    bool flip = false;
    int aa,bb;
    cl_mem cc;
    cl_mem header_mem_obj, a_mem_obj, c_mem_obj;
    cl_kernel kernel;
    cl_command_queue command_queue;
    size_t *global, *local;
    particle *C;

public:

    // Constructor
    VicsekOCL();

    // Constructor
    VicsekOCL(SDL_Renderer* r, int width, int height, float v, float radius, float eta, unsigned int n_particles) : Vicsek(r, width, height, v, radius, eta, n_particles)
    {
        matrix = (particle*)malloc(sizeof(particle)* this->n);

        for(int i=0; i<this->p.size(); i++)
        {
            //std::complex<float> temp = std::polar((float)1.0, this->p[i].dir);

            matrix[i].x = this->p[i].x;
            matrix[i].y = this->p[i].y;
            matrix[i].vx = this->p[i].get_dir_x();//temp.real();
            matrix[i].vy = this->p[i].get_dir_y();//temp.imag();
        }

        // Load the kernel source code into the array source_str
        FILE *fp;
        char *source_str;
        size_t source_size;

        fp = fopen("kernel/kernel.cl", "r");
        if (!fp)
        {
            std::cout << "Failed to load kernel.\n" << std::endl;
            exit(1);
        }

        source_str = (char*)malloc(MAX_SOURCE_SIZE);
        source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
        fclose( fp );

        // Get platform and device information
        cl_platform_id platform_id = NULL;
        cl_device_id device_id = NULL;
        cl_uint ret_num_devices;
        cl_uint ret_num_platforms;

        cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
        cl_platform_id platforms[ret_num_platforms];
        ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);

        std::cout << "Anzahl Platformen: " << ret_num_platforms << std::endl;

        for(int i=0; i<ret_num_platforms; i++)
        {

            char name[1024] = "";
            size_t length = 1024;

            clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 1024, &name, &length);

            std::cout << name << std::endl;

            ret = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &ret_num_devices);

            cl_device_id devices[ret_num_devices] = {};

            ret = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, ret_num_devices, devices, NULL);

            for(int j=0; j<ret_num_devices; j++)
            {
                char name[1024] = "";
                size_t length = 1024;

                clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 1024, &name, &length);

                std::cout <<ret_num_devices << ":" << name << std::endl;
            }


        }

        // Get Device ID
        ret = clGetDeviceIDs( platforms[0], CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

        // Create an OpenCL context
        cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

        // Create a command queue
        this->command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

        // Create memory buffers on the device for each vector
        this->header_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(header), NULL, &ret);
        this->a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(particle) * this->n, NULL, &ret);
        this->c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(particle) * this->n, NULL, &ret);

        // Create a program from the kernel source
        cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

        char options[] = "";//"-cl-unsafe-math-optimizations -cl-mad-enable -cl-no-signed-zeros -cl-finite-math-only -cl-fast-relaxed-math";

        // Build the program
        ret = clBuildProgram(program, 1, &device_id, options, NULL, NULL);

        // Create the OpenCL kernel
        this->kernel = clCreateKernel(program, "vicsek", &ret);

        // Read the memory buffer C on the device to the local variable C
        this->C = (particle*)malloc(sizeof(particle)* this->n);

        // Execute the OpenCL kernel on the list
        //Create the size holders
        this->global = (size_t*) malloc(sizeof(size_t)*1);
        this->local = (size_t*) malloc(sizeof(size_t)*1);

        //Set the size
        this->global[0] = this->n;
        //this->global[1] = n;
        this->local [0] = 4;
        //this->local [1] = 4;

    }

    void createQuadTree();

    void hightlightNeighbours(int x, int y);

    void getNeighbours(int x, int y, std::vector<Particle*> &np);

    Step();

    Draw(SDL_Renderer* r);
};

#endif // VICSEKOCL_H_INCLUDED
