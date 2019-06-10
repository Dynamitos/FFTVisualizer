#pragma once
#include "AudioProcessor.h"
#include "WindowFunction.h"
#include "RectangularWindow.h"
#include "HammingWindow.h"
#include "HannWindow.h"
#include "CosineWindow.h"
#include "TriangularWindow.h"
#include "BartlettHannWindow.h"
#include "BartlettWindow.h"
#include "LanczosWindow.h"
#include "BlackmanWindow.h"
#include "GaussWindow.h"
#include <vector>
class FourierTransform : public AudioProcessor
{
public:
	static const WindowFunction* NONE;
	static const WindowFunction* HAMMING;
	static const WindowFunction* HANN;
	static const WindowFunction* COSINE;
	static const WindowFunction* TRIANGULAR;
	static const WindowFunction* BARTLETT;
	static const WindowFunction* BARTLETTHANN;
	static const WindowFunction* LANCZOS;
	static const WindowFunction* BLACKMAN;
	static const WindowFunction* GAUSS;

protected:
	static const int LINAVG = 1;
	static const int LOGAVG = 2;
	static const int NOAVG = 3;

	int timeSize;
	int sampleRate;
	float bandWidth;
	WindowFunction* currentWindow;
	float* real = nullptr;
	float* imag = nullptr;
	float* spectrum = nullptr;
	float* averages = nullptr;
	
	int currentBufferIndex;
	std::vector<float> sampleBuffer;
	
	int whichAverage;
	int octaves;
	int avgPerOctave;
	
	int realLength;
	int imagLength;
	int spectrumLength;
	int averagesLength;

	FourierTransform(int bufferSize, float sampleRate);
	virtual ~FourierTransform();

	virtual void allocateArrays() = 0;

	virtual void init();

	void setComplex(float * r, int realLength, float * i, int imagLength);

	void fillSpectrum();

public:
	void noAverages();

	void linAverages(int numAvg);

	void logAverages(int minBandwidth, int bandsPerOctave);

	void window(WindowFunction* windowFunction);

	void doWindow(float* samples, int length);

	int getTimeSize();

	int getSpecSize();

	float getBand(int i);

	float getBandWidth();

	float getAverageBandWidth(int averageIndex);

	virtual void setBand(int i, float a) = 0;

	virtual void scaleBand(int i, float s) = 0;

	int freqToIndex(float freq);

	float indexToFreq(int i);

	float getAverageCenterFrequency(int i);

	float getFreq(float freq);

	void setFreq(float freq, float a);

	void scaleFreq(float freq, float s);

	int getAvgSize();

	float getAvg(int i);

	float calcAvg(float lowFreq, float hiFreq);

	float* getSpectrumReal();

	float* getSpectrumImaginary();

	virtual void finalizeConverterInfo(ConverterInitInfo& initInfo);

	virtual void forward(const ConvertedSampleContainer& container);

	virtual void forward(float* buffer, int bufferLength) = 0;

	virtual void forward(float* buffer, int bufferLength, int startAt);

	//virtual void inverse(float* buffer) = 0;

	//void inverse(float* freqReal, float* freqImag, float* buffer);
};