#pragma once
#include "WindowFunction.h"

class BartlettWindow : public WindowFunction
{
public:
	BartlettWindow()
	{
	}
	~BartlettWindow()
	{

	}
protected:
	float value(int length, int index)
	{
		return 2.f / (length - 1) * ((length - 1) / 2.f - abs(index - (length - 1) / 2.f));
	}
};