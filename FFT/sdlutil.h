#pragma once
#include <SDL/SDL.h>
#include <vector>
#include <iostream>

class Display
{
public:
	Display(int width, int height);
	~Display();
	void drawFFT(float* fftOut, int count);
private:
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Renderer* renderer;
	int width, height;
	std::vector<SDL_Point> points;
};