#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include "Vicsek.h"
#include "VicsekQT.h"
#include "VicsekQTMT.h"

void Draw(SDL_Renderer *renderer, VicsekQTMT& quad);

void Draw_Text(SDL_Renderer *r, int x, int y, std::string s);

void HSV_TO_RGB(float H, float S, float V, unsigned short &R, unsigned short &G, unsigned short &B);

uint64_t millis();

#endif // FUNCTIONS_H_INCLUDED
