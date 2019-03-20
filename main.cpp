#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <mutex>

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

int start_time = millis();
int frames = 0;

int main( int argc, char* args[] )
{
    // Parameter START / END / NUMBER OF STEPS
    v =         Parameter(1,    0.1,  0);
    eta =       Parameter(0.0,    2*M_PI,  20);
    radius =    Parameter(50,    10, 0);
    n =         Parameter(500, 0,  0);

    int max_iterations = 500;

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

            //Vicsek sim = Vicsek(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v, radius, eta, n); // Vicsek-Model with naive neighbour search O(n^2)
            //VicsekQT sim = VicsekQT(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.begin, radius.begin, eta.begin, (int)n.begin); // Vicsek-Model using QuadTree for neighbour search O(log(n))
            VicsekQTMT sim = VicsekQTMT(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart()); // Vicsek-Model using QuadTree for neighbour search O(log(n)) and Multi-Threading

            std::cout << "v: " << v.getCurrent() << std::endl;
            std::cout << "eta: " << eta.getCurrent() << std::endl;
            std::cout << "radius: " << radius.getCurrent() << std::endl;
            std::cout << "n: " << n.getCurrent() << std::endl;

            std::thread t1(Draw, renderer, std::ref(sim));

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

                        if(highlight_mouse_neighbours)
                        {
                            mm.lock();
                        }
                        sim.Step();

                        if(highlight_mouse_neighbours)
                        {
                            sim.hightlightNeighbours(mouse_x, mouse_y);
                        }

                        if(highlight_mouse_neighbours)
                        {
                            mm.unlock();
                        }

                        res.addEntry(SCREEN_WIDTH,
                                     SCREEN_HEIGHT,
                                     n.getCurrent() / (SCREEN_WIDTH * SCREEN_HEIGHT),
                                     sim.step_count,
                                     n.getCurrent(),
                                     sim.calc_avg_norm_vel(),
                                     v.getCurrent(),
                                     sim.getEta(),
                                     radius.getCurrent());

                        frames++;
                        int runtime = millis() - start_time;

                        if(runtime >= 100)
                        {
                            CPS = (frames / (float(runtime)/1000));
                            frames = 0;
                            start_time = millis();
                        }
                    }

                    ResultEntry avg = res.getAvgLastN(100);
                    res_avg.addEntry(avg.world_width,
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

