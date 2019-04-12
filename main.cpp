#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <cstddef>

#include <SDL.h>
#include "SDL_ttf.h"
#include <SDL_image.h>

#include "Vicsek.h"
#include "VicsekQT.h"
#include "VicsekQTMT.h"
#include "Result.h"
#include "functions.h"
#include "Parameter.h"

//Screen dimension constants
int SCREEN_WIDTH = 512;
int SCREEN_HEIGHT = 512;

//Vicsek model constants
Parameter v; // Velocity v_0
Parameter eta; // noise parameter from 0 to 2*pi
Parameter radius; // interaction radius of particles
Parameter n; // Quantity of particles

unsigned short font_size = 16;

using namespace std;

int mouse_x;
int mouse_y;

float CPS;
float FPS;

Result res = Result();
Result res_avg = Result();

TTF_Font* font;

bool highlight_mouse_neighbours = false;

std::mutex mm;

unsigned long long start_time = micros();
unsigned int frames = 0;

int main( int argc, char* args[] )
{
    // Parameter START / END / NUMBER OF STEPS
    v =         Parameter(1,    0.1,    0);
    eta =       Parameter(0.4,  2*M_PI, 10);
    radius =    Parameter(20,   10,     0);
    n =         Parameter(10000,  500,      0); // MAX 65536

    int max_iterations = 1000000;
    int avg_count = 100;

    //The window we'll be rendering to
    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }

    else
    {
        TTF_Init();
        IMG_Init(IMG_INIT_PNG);

        font = TTF_OpenFont("fonts/MorePerfectDOSVGA.ttf", font_size);

        if (font == NULL)
        {
            printf("Unable to load font: %s %s \n", TTF_GetError());
            // Handle the error here.
        }

        //Create window
        window = SDL_CreateWindow( "Flocking", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH+250, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }

        else
        {

            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            //Vicsek sim = Vicsek(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart()); // Vicsek-Model with naive neighbour search O(n^2)
            //VicsekQT sim = VicsekQT(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart()); // Vicsek-Model using QuadTree for neighbour search O(log(n))
            //VicsekQTMT sim = VicsekQTMT(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart()); // Vicsek-Model using QuadTree for neighbour search O(log(n)) and Multi-Threading
            VicsekOCL sim = VicsekOCL(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart());

            std::thread t1(Draw, renderer, std::ref(sim));

            n.reset();
            while(true)
            {
                //mm.lock();
                //sim.setParticleCount((int)n.getCurrent());
                //mm.unlock();

                eta.reset();
                while(true)
                {
                    sim.setEta(eta.getCurrent());

                    radius.reset();
                    while(true)
                    {
                        sim.radius = radius.getCurrent();

                        while(sim.step_count < max_iterations)
                        {
                            SDL_Event event;

                            while (SDL_PollEvent(&event))
                            {


                                SDL_GetMouseState(&mouse_x, &mouse_y);

                                if (event.type == SDL_KEYDOWN)
                                {
                                    switch(event.key.keysym.sym)
                                    {
                                    case SDLK_h:
                                        if(highlight_mouse_neighbours)
                                        {
                                            highlight_mouse_neighbours = false;
                                        }

                                        else
                                        {
                                            highlight_mouse_neighbours = true;
                                        }
                                        break;
                                    default:
                                        break;
                                    }
                                }
                            }

                            //mm.lock();

                            sim.Step();

                            if(highlight_mouse_neighbours)
                            {
                                sim.highlightNeighbours(mouse_x, mouse_y);
                            }

                            //mm.unlock();

                            res.addEntry(CPS,
                                         SCREEN_WIDTH,
                                         SCREEN_HEIGHT,
                                         n.getCurrent() / (SCREEN_WIDTH * SCREEN_HEIGHT),
                                         sim.step_count,
                                         n.getCurrent(),
                                         sim.calc_avg_norm_vel(),
                                         v.getCurrent(),
                                         sim.getEta(),
                                         radius.getCurrent());

                            frames++;
                            unsigned long long runtime = micros() - start_time;
                            if(runtime > 1000000)
                            {
                                CPS = frames / (runtime/1000000.0);
                                frames = 0;
                                start_time = micros();
                            }



                        }

                        ResultEntry avg = res.getAvgLastN(avg_count);
                        res_avg.addEntry(avg.cps,
                                         avg.world_width,
                                         avg.world_height,
                                         avg.density,
                                         res.data.size(),
                                         avg.n_particles,
                                         avg.avg_norm_vel,
                                         avg.v,
                                         avg.eta,
                                         avg.radius);

                        res.clear();
                        sim.reset();

                        radius.next();

                        if(radius.isFinished())
                        {
                            break;
                        }
                    }

                    eta.next();

                    if(eta.isFinished())
                    {
                        break;
                    }
                }
                n.next();

                if(n.isFinished())
                {
                    break;
                }

            }

            // Save simulation result to file
            res_avg.saveToFile();
        }
    }

//Destroy window
    SDL_DestroyWindow( window );

//Quit SDL subsystems
    SDL_Quit();

    return 0;
}

