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
#include <SDL_ttf.h>
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

// factor to scale parameters instead of changing the map size
float multi = 11.3;

int main( int argc, char* args[] )
{
    // Parameter START / END / NUMBER OF STEPS
    v       = Parameter(0.05 * multi,    1 * multi, 0);       // Velocity
    eta     = Parameter(0.1,  1.0, 0);                        // noise - ([0, 1] -> [0 , pi])
    radius  = Parameter(1 * multi,   10 * multi, 0);          // interaction radius
    n       = Parameter(8192,  500, 0);                       // Particle count

    int max_iterations = 1000000;
    int avg_count = (int)(max_iterations*0.1);

    //The window we'll be rendering to
    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    }

    else
    {
        TTF_Init();
        IMG_Init(IMG_INIT_PNG);

        font = TTF_OpenFont("fonts/MorePerfectDOSVGA.ttf", font_size);

        if (font == NULL)
        {
            cout << "Unable to load font: " << TTF_GetError() << endl;
        }

        //Create window
        window = SDL_CreateWindow( "Vicsek-model-simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH+250, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        }

        else
        {

            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            // You can choose which Vicsek implementation you use

            // only naive implementation of the model
            //Vicsek sim = Vicsek(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart()); // Vicsek-Model with naive neighbour search O(n^2)

            // QuadTree
            //VicsekQT sim = VicsekQT(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart()); // Vicsek-Model using QuadTree for neighbour search O(log(n))

            // Multi threading with QuadTree
            //VicsekQTMT sim = VicsekQTMT(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart()); // Vicsek-Model using QuadTree for neighbour search O(log(n)) and Multi-Threading

            // hardware acceleration with graphics card (OpenCL)
            VicsekOCL sim = VicsekOCL(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, v.getStart(), radius.getStart(), eta.getStart(), (int)n.getStart());

            std::thread t1(Draw, renderer, std::ref(sim));

            // reset particle count
            n.reset();
            while(true)
            {
                //mm.lock();
                //sim.setParticleCount((int)n.getCurrent());
                //mm.unlock();

                // reset noise
                eta.reset();
                while(true)
                {
                    // set current simulation noise parameter
                    sim.setEta(eta.getCurrent());

                    radius.reset();
                    while(true)
                    {
                        // set current simulation radius parameter
                        sim.radius = radius.getCurrent();

                        // simulate until maximum iteration count
                        while(sim.step_count < max_iterations)
                        {

                            // catch SDL events
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

                            // lock for other threads
                            mm.lock();

                            // execute one simulation
                            sim.Step();

                            // highlight neighbours near the mouse pointer
                            if(highlight_mouse_neighbours)
                            {
                                sim.highlightNeighbours(mouse_x, mouse_y);
                            }

                            // unlock for other threads
                            mm.unlock();

                            // add an entry to result set
                            res.addEntry(CPS,
                                         SCREEN_WIDTH,
                                         SCREEN_HEIGHT,
                                         n.getCurrent() / ((SCREEN_WIDTH/multi) * (SCREEN_HEIGHT/multi)),
                                         sim.step_count,
                                         n.getCurrent(),
                                         sim.calc_avg_norm_vel(),
                                         v.getCurrent(),
                                         sim.getEta(),
                                         radius.getCurrent());

                            // calculates the speed of simulation
                            frames++;
                            unsigned long long runtime = micros() - start_time;
                            if(runtime > 1000000)
                            {
                                CPS = frames / (runtime/1000000.0);
                                frames = 0;
                                start_time = micros();
                            }



                        }

                        // add averaged simulations results to result set
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

                        // clear Result set which calcs the average
                        res.clear();

                        // resets the simulation to prepare for changed parameters
                        sim.reset();

                        // iterate to next radius parameter
                        radius.next();

                        // leave when radius parameters is at maximum
                        if(radius.isFinished())
                        {
                            break;
                        }
                    }

                    // iterate to next noise value
                    eta.next();

                    // leave when noise parameters is at maximum
                    if(eta.isFinished())
                    {
                        break;
                    }
                }
                // iterate to next particle count value
                n.next();

                // leave when particle parameters is at maximum
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

