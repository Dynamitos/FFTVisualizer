#include "sdlutil.h"
#include <chrono>


SDLDisplay::SDLDisplay()
{
}

SDLDisplay::~SDLDisplay()
{
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void SDLDisplay::init(AudioVisualizerInfo visualizerInfo)
{
	width = visualizerInfo.screenDimensions.x;
	height = visualizerInfo.screenDimensions.y;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Error initializing SDL" << std::endl;
	}
	window = SDL_CreateWindow(visualizerInfo.name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN); 
	if (window == NULL) 
	{ 
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
	}

	screenSurface = SDL_GetWindowSurface(window);

	renderer = SDL_CreateRenderer(window, -1, 0);
	
	SDL_UpdateWindowSurface(window);
}

void SDLDisplay::renderData(std::unique_ptr<SampleContainer> data)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0xff, 0, SDL_ALPHA_OPAQUE);

	if (points.size() < data->numSamples)
	{
		points.resize(data->numSamples);
	}

	float step = width / (float)data->numSamples;
	float current = step/2.0f;
	float* fftData = (float*)data->convertedSamples;
	for (uint32_t i = 0; i < data->numSamples; ++i)
	{
		points[i].x = (int)current;
		points[i].y = (int)(height/2) - (10*fftData[i]);
		current += step;
	}

	SDL_RenderDrawLines(renderer, points.data(), data->numSamples);
	
	SDL_UpdateWindowSurface(window);

	SDL_RenderPresent(renderer);

	SDL_PollEvent(nullptr);
}

bool SDLDisplay::shouldClose()
{
	return false;
}