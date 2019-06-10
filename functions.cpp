#include <iostream>
#include <chrono>
#include <string>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <cstddef>

#include <SDL.h>
#include "SDL_ttf.h"
#include <SDL_image.h>

#include "Vicsek.h"
#include "VicsekQT.h"
#include "VicsekQTMT.h"
#include "functions.h"
#include "Result.h"
#include "Parameter.h"

extern bool highlight_mouse_neighbours;

extern int mouse_x;
extern int mouse_y;

extern float CPS;
extern float FPS;

extern Result res;
extern Result res_avg;

extern const unsigned short font_size;

extern Parameter v;
extern Parameter eta;
extern Parameter radius;
extern Parameter n;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern TTF_Font* font;
extern std::mutex mm;

// Draw everything to the renderer

void Draw(SDL_Renderer *renderer, Vicsek& quad)
{
    int frames = 0;
    long long start_time = micros();

    while(1)
    {
        mm.lock();

        SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0x0 );
        SDL_RenderClear( renderer );

        quad.Draw(renderer);

        SDL_Rect rect;
        rect.x = SCREEN_WIDTH;
        rect.y = 0;
        rect.h = SCREEN_HEIGHT;
        rect.w = SCREEN_WIDTH;

        SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0x0 );
        SDL_RenderFillRect(renderer, &rect);

        std::stringstream s;
        s << std::fixed << std::setprecision(3) << "Avg. norm. vel.: "<< quad.calc_avg_norm_vel();
        std::string ss = s.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10, ss);

        std::stringstream s3;
        s3 << std::fixed << std::setprecision(3) << "Radius: "<< quad.radius << " / " << radius.getEnd();
        ss = s3.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(1*font_size), ss);

        std::stringstream s1;
        s1 << std::fixed << std::setprecision(3) << "Noise: "<< quad.getEta() << " / " << eta.getEnd();
        ss = s1.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+2*font_size, ss);

        std::stringstream s2;
        s2 << std::fixed << std::setprecision(3) << "Particles: "<< quad.n;
        ss = s2.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(3*font_size), ss);

        std::stringstream s4;
        s4 << std::fixed << std::setprecision(3) << "Messurements: "<< res_avg.data.size();
        ss = s4.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(4*font_size), ss);

        std::stringstream s9;
        s9 << std::fixed << std::setprecision(3) << "Messurements Set: "<< res.data.size();
        ss = s9.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(9*font_size), ss);

        std::stringstream s5;
        s5 << std::fixed << std::setprecision(3) << "CPS: "<< CPS;
        ss = s5.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(5*font_size), ss);

        std::stringstream s6;
        s6 << std::fixed << std::setprecision(3) << "FPS: "<< FPS;
        ss = s6.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(6*font_size), ss);

        std::stringstream s7;
        s7 << std::fixed << std::setprecision(3) << "Iterations: "<< quad.step_count;
        ss = s7.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(7*font_size), ss);

        std::stringstream s8;
        s8 << std::fixed << std::setprecision(3) << "Mouse Pos.: "<< mouse_x << " / " << mouse_y;
        ss = s8.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(8*font_size), ss);

        std::stringstream s11;
        s11 << std::fixed << std::setprecision(3) << "Avg. last 100: " << res.getAvgLastN(100).avg_norm_vel;
        ss = s11.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(11*font_size), ss);

        std::stringstream s12;
        if(highlight_mouse_neighbours)
        {
            s12 << std::fixed << std::setprecision(3) << "Highlight Neighb.[H]: ON";
        }
        else
        {
            s12 << std::fixed << std::setprecision(3) << "Highlight Neighb.[H]: OFF";

        }

        ss = s12.str();
        Draw_Text(renderer, SCREEN_WIDTH+20, 10+(12*font_size), ss);

        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderDrawLine(renderer, SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer, SCREEN_WIDTH, SCREEN_HEIGHT/2, SCREEN_WIDTH+250, SCREEN_HEIGHT/2);

        //int circumference = 2 * 110 * M_PI;
        int boxes = 2 * 110 * M_PI;
        std::vector<int> histogram(boxes, 0);

        for(int i=0; i<quad.p.size(); i++)
        {
            int box = floor((quad.p[i].get_dir()/(2.0*M_PI)) * boxes);

            histogram[box%boxes]++;
        }


        int center_hist_x = 637;
        int center_hist_y = 387;


        for(int i=1; i < histogram.size()-1; i++)
        {
            histogram[i] = (histogram[i-1] + histogram[i] + histogram[i+1]) / 3;
        }

        int max = 0;

        for(int i=0; i < histogram.size(); i++)
        {
            if(histogram[i] > max)
            {
                max = histogram[i];
            }
        }

        for(int i=0; i < histogram.size(); i++)
        {

            unsigned short r,g,b;
            HSV_TO_RGB((i/110.0 / (2*M_PI)) * 360.0, 1.0, 0.4, r, g, b);
            SDL_SetRenderDrawColor(renderer, r, g, b, 0);

            float n_x = cos(i/110.0);
            float n_y = sin(i/110.0);

            float m = 60.0;
            int hx1 = (80.0-(m/2.0)) * n_x + center_hist_x;
            int hy1 = (80.0-(m/2.0)) * n_y + center_hist_y ;
            int hx2 = (80.0+(m/2.0)) * n_x + center_hist_x;
            int hy2 = (80.0+(m/2.0)) * n_y + center_hist_y;
            SDL_RenderDrawLine(renderer, hx1, hy1, hx2, hy2);

            HSV_TO_RGB((i/110.0 / (2*M_PI)) * 360.0, 1.0, 1.0, r, g, b);
            SDL_SetRenderDrawColor(renderer, r, g, b, 0);

            m = 60.0 * (float)((float)histogram[i] / (float)max);
            hx1 = (80.0-(m/2.0)) * n_x + center_hist_x;
            hy1 = (80.0-(m/2.0)) * n_y + center_hist_y ;
            hx2 = (80.0+(m/2.0)) * n_x + center_hist_x;
            hy2 = (80.0+(m/2.0)) * n_y + center_hist_y;
            SDL_RenderDrawLine(renderer, hx1, hy1, hx2, hy2);
        }

        SDL_RenderPresent( renderer );

        frames++;
        int runtime = micros() - start_time;

        if(runtime >= 1000000)
        {
            FPS = (frames / (float(runtime)/1000000));
            frames = 0;
            start_time = micros();
        }

        mm.unlock();

    }


}

// Draw Text to SDL Renderer

void Draw_Text(SDL_Renderer *r, int x, int y, std::string s)
{
    SDL_Rect dstrect;
    SDL_Color tmpfontcolor = {255,255,255,0};
    SDL_Color tmpfontbgcolor = {0, 0, 0, 0};

    const char* c = s.c_str();

    SDL_Surface* surface = TTF_RenderText_Solid(font, c, tmpfontcolor);

    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = surface->clip_rect.w;
    dstrect.h = surface->clip_rect.h;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(r, surface);
    SDL_RenderCopy(r, texture, &surface->clip_rect, &dstrect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// get milliseconds

uint64_t millis()
{
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return ms;
}

uint64_t micros()
{
    uint64_t ms = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return ms;
}


// convert HSV color to RGB color

void HSV_TO_RGB(float H, float S, float V, unsigned short &R, unsigned short &G, unsigned short &B)
{
    float h_i = H / 60.0;
    float f = h_i - (int)h_i;
    float p = V * (1 - S);
    float q = V * (1 - S * f);
    float t = V * (1 - S * (1-f));

    switch ((int)h_i)
    {
    case 1:
        R = q * 255;
        G = V * 255;
        B = p * 255;
        break;
    case 2:
        R = p * 255;
        G = V * 255;
        B = t * 255;
        break;
    case 3:
        R = p * 255;
        G = q * 255;
        B = V * 255;
        break;
    case 4:
        R = t * 255;
        G = p * 255;
        B = V * 255;
        break;
    case 5:
        R = V * 255;
        G = p * 255;
        B = q * 255;
        break;
    default:
        R = V * 255;
        G = t * 255;
        B = p * 255;
    }
}
