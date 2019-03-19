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

#include "Vicsek.h"
#include "VicsekQT.h"
#include "VicsekQTMT.h"
#include "Result.h"

//Screen dimension constants
const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;

//Vicsek model constants
const float v = 0.1; // Velocity v_0
float eta = 0.0; // noise parameter from 0 to 2*pi
const float radius = 50; // interaction radius of particles
const unsigned int n = 2000; // Quantity of particles

using namespace std;

int mouse_x;
int mouse_y;

TTF_Font* font;

std::mutex mm;

void Draw(SDL_Renderer *renderer, Vicsek& quad)
{
    while(1)
    {

        stringstream s;

        SDL_Rect dstrect;

        SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0x0 );
        SDL_RenderClear( renderer );

        mm.lock();
        quad.Draw(renderer);
        mm.unlock();


        SDL_Color tmpfontcolor = {255,255,255,0};
        SDL_Color tmpfontbgcolor = {0, 0, 0, 0};
        SDL_Surface* resulting_text;

        s << std::fixed << std::setprecision(3) <<quad.calc_avg_norm_vel();
        string ss = s.str();
        const char * c = ss.c_str();

        SDL_Surface* surface = TTF_RenderText_Solid(font, c, tmpfontcolor);

        dstrect.x = 0;
        dstrect.y = 550;
        dstrect.w = surface->clip_rect.w;
        dstrect.h = surface->clip_rect.h;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, &surface->clip_rect, &dstrect);

        SDL_RenderPresent( renderer );

    }

}

uint64_t millis()
{
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return ms;
}

int start_time = millis();
int frames = 0;

string sim_type = "";
int sim_thread_count = 8;

int main( int argc, char* args[] )
{


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

        font = TTF_OpenFont("font/Enigma__2.ttf", 20);

        if (font == NULL)
        {
            printf("Unable to load font: %s %s \n", TTF_GetError());
            // Handle the error here.
        }

        //Create window
        window = SDL_CreateWindow( "Flocking", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT+70, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }

        else
        {

            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            Result res = Result();
            Result res_avg = Result();

            Vicsek sim = Vicsek(SCREEN_WIDTH, SCREEN_HEIGHT, v, radius, eta, n); // Vicsek-Model with naive neighbour search O(n^2)
            //VicsekQT sim = VicsekQT(SCREEN_WIDTH, SCREEN_HEIGHT, v, radius, eta, n); // Vicsek-Model using QuadTree for neighbour search O(log(n))
            //VicsekQTMT sim = VicsekQTMT(SCREEN_WIDTH, SCREEN_HEIGHT, v, radius, eta, n); // Vicsek-Model using QuadTree for neighbour search O(log(n)) and Multi-Threading

            std::thread t1(Draw, renderer, std::ref(sim));

            for(int q=0; q<10; q++)
            {
                eta += 0.2;
                cout << "q: " << q << endl;
                //bool isStable = false;

                while(1)//sim.step_count < 10)
                {
                    SDL_Event event;

                    while (SDL_PollEvent(&event))
                    {
                        SDL_GetMouseState(&mouse_x, &mouse_y);

                    }

                    int start_sim_time = millis();

                    mm.lock();
                    sim.Step();
                    res.addEntry(sim.step_count, n, sim.calc_avg_norm_vel(), v, eta, radius);


                    std::vector<Particle*> neighbours;
                    sim.getNeighbours(mouse_x, mouse_y, neighbours);

                    for(int n_np=0; n_np<neighbours.size(); n_np++)
                    {
                        neighbours[n_np]->color_r = 255;
                        neighbours[n_np]->color_g = 0;
                        neighbours[n_np]->color_b = 0;
                    }
                    mm.unlock();


                    int sim_time = millis() - start_sim_time;

                    frames++;
                    int runtime = millis() - start_time;

                    if(runtime >= 1000)
                    {
                        float cps = (frames / (float(runtime)/1000));
                        cout << "Average Normalized Vel.: " << sim.calc_avg_norm_vel() << endl;
                        cout << "CPS: " << cps << endl;
                        cout << "Simulation time: " << sim_time << "ms"<< endl;
                        cout << "Simulation steps: " << sim.step_count << endl;
                        cout << "Mouse: " << mouse_x << ":" << mouse_y << endl;
                        frames = 0;
                        start_time = millis();
                        //res.isStable();
                    }

                    //isStable = res.isStable();
                }

                ResultEntry avg = res.getAvgLastN(5);
                res_avg.addEntry(avg.n_iterations, avg.n_particles, avg.avg_norm_vel, avg.v, avg.eta, avg.radius);
                res.clear();
                sim.reset();
                sim.eta = eta;
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

