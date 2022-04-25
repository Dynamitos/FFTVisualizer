#pragma once
#include "WindowFunction.h"
#include <assert.h>

class GaussWindow : public WindowFunction
{
	double alpha;

public:
	GaussWindow(double alpha)
	{
		if (alpha < 0.0f || alpha >0.5f)
		{
			assert(0);
		}
		this->alpha = alpha;
	}
	GaussWindow()
		: GaussWindow(0.25f)
	{

	}
	~GaussWindow()
	{

	}
protected:
	float value(int length, int index)
	{
		return (float)pow(M_E, -0.5f * pow((index - ((int64_t)length - 1) / (double)2) / (this->alpha * ((int64_t)length - 1) / (double)2), (double)2));
	}
};