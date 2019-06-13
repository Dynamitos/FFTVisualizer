#pragma once
#include <SDL/SDL.h>
#include <vector>
#include <iostream>
#include "Resources.h"
#include "PacketQueue.h"

class Display
{
public:
	Display(int width, int height);
	~Display();
	void run(PacketQueue<SampleContainer>* inputQueue)
	{
		static auto start = std::chrono::high_resolution_clock::now();
		while (!inputQueue->isFinished())
		{
			auto end = std::chrono::high_resolution_clock::now();
			int64_t delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
			//acquire ownership
			std::unique_ptr<SampleContainer> input = inputQueue->popPacket();
			std::this_thread::sleep_for(std::chrono::nanoseconds(input->timeStamp - delta));

			//this one is fairly sketch, but for now we know that it is float data
			float* fftData = (float*)input->convertedSamples;
			drawFFT(fftData, input->dataSize / sizeof(float));
		}
	}
	void drawFFT(float* fftOut, int count);
private:
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Renderer* renderer;
	int width, height;
	std::vector<SDL_Point> points;
};