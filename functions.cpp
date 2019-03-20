#include <iostream>
#include <chrono>
#include <string>
#include <mutex>
#include <sstream>
#include <iomanip>

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

void Draw(SDL_Renderer *renderer, VicsekQTMT& quad)
{
    int frames = 0;
    long long start_time = millis();

    while(1)
    {

        SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0x0 );
        SDL_RenderClear( renderer );

        mm.lock();
        quad.Draw(renderer);
        mm.unlock();

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
        s1 << std::fixed << std::setprecision(3) << "eta: "<< quad.getEta() << " / " << eta.getEnd();
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

        SDL_RenderPresent( renderer );

        frames++;
        int runtime = millis() - start_time;

        if(runtime >= 100)
        {
            FPS = (frames / (float(runtime)/1000));
            frames = 0;
            start_time = millis();
        }

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
