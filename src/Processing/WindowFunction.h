#pragma once
#include <math.h>
#define TWO_PI M_PI * 2.0f
class WindowFunction
{
protected:
	int length;
public:
	WindowFunction() : length{ 0 }
	{}
	virtual ~WindowFunction()
	{}

	void apply(float* samples, int length)
	{
		this->length = length;
		for (int n = 0; n < length; ++n)
		{
			samples[n] *= value(length, n);
		}
	}

	void apply(float* samples, int offset, int length)
	{
		this->length = length;
		for (int n = offset; n < offset + length; ++n)
		{
			samples[n] *= value(length, n - offset);
		}
	}

	float* generateCurve(int length)
	{
		float* samples = new float[length];
		for (int n = 0; n < length; ++n)
		{
			samples[n] = 1.f * value(length, n);
		}
		return samples;
	}

protected:
	virtual float value(int length, int index) = 0;
};
