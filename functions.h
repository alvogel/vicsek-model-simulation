#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include "Vicsek.h"
#include "VicsekQT.h"
#include "VicsekQTMT.h"
#include "VicsekOCL.h"

// draw to renderer

void Draw(SDL_Renderer *renderer, Vicsek& quad);

// draw TTF text

void Draw_Text(SDL_Renderer *r, int x, int y, std::string s);

// convert HSV to RGB

void HSV_TO_RGB(float H, float S, float V, unsigned short &R, unsigned short &G, unsigned short &B);

// return milliseconds

uint64_t millis();

// return microseconds

uint64_t micros();

#endif // FUNCTIONS_H_INCLUDED
