#pragma once

#include "FourierTransform.h"

class FFT : public FourierTransform
{
public:
	FFT(int timeSize, float sampleRate);
	~FFT();
protected:
	void allocateArrays();

	void scaleBand(int i, float s);

	void setBand(int i, float a);

private:
	void fft();
	
public:
	void forward(float* buffer, int bufferLength);

	void forward(float* buffer, int bufferLength, int startAt);

	void forward(float* buffReal, int rLength, float* buffImag, int iLength);

private:
	int* reverse = nullptr;

	void buildReverseTable();

	void bitReverseSamples(float* samples, int sampleSize, int startAt);

	void bitReverseComplex();

	float* sinlookup = nullptr;
	float* coslookup = nullptr;

	float fastsin(int i);

	float fastcos(int i);

	void buildTrigTables();
};