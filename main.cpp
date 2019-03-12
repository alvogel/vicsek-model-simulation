#include <iostream>
#include <chrono>
#include <thread>

#include <SDL.h>

#include "Vicsek.h"
#include "VicsekQT.h"
#include "VicsekQTMT.h"

//Screen dimension constants
const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;

//Vicsek model constants
const float v = 1; // Velocity v_0
const float eta = 1; // from 0 to 2*pi
const float radius = 5; //
const unsigned int n = 10000; // Quantity of particles

using namespace std;

void Draw(SDL_Renderer *renderer, VicsekQT& quad)
{
    while(1)
    {
        SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0x0 );
        SDL_RenderClear( renderer );
        quad.Draw(renderer);
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

int main( int argc, char* args[] )
{
    //Vicsek sim = Vicsek(SCREEN_WIDTH, SCREEN_HEIGHT, v, radius, n); // Vicsek-Model with naive neighbour search O(n^2)
    VicsekQT sim = VicsekQT(SCREEN_WIDTH, SCREEN_HEIGHT, v, radius, n); // Vicsek-Model using QuadTree for neighbour search O(log(n))
    //VicsekQTMT sim = VicsekQTMT(SCREEN_WIDTH, SCREEN_HEIGHT, v, radius, n); // Vicsek-Model using QuadTree for neighbour search O(log(n)) and Multi-Threading

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

        //Create window
        window = SDL_CreateWindow( "Flocking", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }

        else
        {

            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            std::thread t1(Draw, renderer, std::ref(sim));

            while(1)
            {
                SDL_Event event;

                while (SDL_PollEvent(&event))
                {

                }

                int start_sim_time = millis();

                sim.Step();
                /*
                                SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0x0 );
                                SDL_RenderClear( renderer );
                                sim.Draw(renderer);

                                SDL_RenderPresent( renderer );
                */
                int sim_time = millis() - start_sim_time;

                frames++;
                int runtime = millis() - start_time;

                if(runtime >= 1000)
                {
                    float cps = (frames / (float(runtime)/1000));
                    cout << "Average Normalized Vel.: " << sim.calc_avg_norm_vel() << endl;
                    cout << "CPS: " << cps << endl;
                    cout << "Simulation time: " <<sim_time << "ms"<< endl;
                    //int sum = sim.qt1.size()+sim.qt2.size()+sim.qt3.size()+sim.qt4.size()+sim.qt5.size()+sim.qt6.size()+sim.qt7.size()+sim.qt8.size();
//                    cout << "Particle im QuadTree:" << sum << endl;
                    //cout << "QuadTree Nodes:" << sim.qt.nodes() << endl;
                    //cout << "Gelöscht:" << sim.qt.deleted << endl;
                    frames = 0;
                    start_time = millis();
                }
            }
        }
    }

    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}

