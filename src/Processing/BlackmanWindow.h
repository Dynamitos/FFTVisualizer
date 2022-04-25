#pragma once
#include "WindowFunction.h"

class BlackmanWindow : public WindowFunction
{
protected:
	float alpha;

public:
	BlackmanWindow(float alpha)
	{
		this->alpha = alpha;
	}

	BlackmanWindow()
		: BlackmanWindow(0.16f)
	{
		
	}
	~BlackmanWindow()
	{

	}

protected:
	float value(int length, int index)
	{
		float a0 = (1 - this->alpha) / 2.f;
		float a1 = 0.5f;
		float a2 = this->alpha / 2.f;

		return a0 - a1 * (float)cos(TWO_PI * index / (static_cast<double>(length) - 1.0f)) + a2 * (float)cos(4 * M_PI * index / (static_cast<double>(length) - 1.0f));
	}
};
