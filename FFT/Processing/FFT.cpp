#include "FFT.h"

FFT::FFT()
	: FourierTransform()
{
}

FFT::~FFT()
{
	if (sinlookup != nullptr) delete sinlookup;
	if (coslookup != nullptr) delete coslookup;
}

void FFT::allocateArrays()
{
	spectrum = new float[static_cast<uint64_t>(timeSize / 2) + 1];
	real = new float[timeSize];
	imag = new float[timeSize];
	realLength = timeSize;
	imagLength = timeSize;
	spectrumLength = timeSize / 2 + 1;
}

void FFT::scaleBand(int i, float s)
{
	if (s < 0)
	{
		printf("Can't scale a frequency band by a negative value.");
		return;
	}

	real[i] *= s;
	imag[i] *= s;
	spectrum[i] *= s;

	if (i != 0 && i != timeSize / 2)
	{
		real[timeSize - i] = real[i];
		imag[timeSize - i] = -imag[i];
	}
}

void FFT::setBand(int i, float a)
{
	if (a < 0)
	{
		printf("Can't set a frequency band to a negative value.");
	}
	if (real[i] == 0 && imag[i] == 0)
	{
		real[i] = a;
		spectrum[i] = a;
	}
	else
	{
		real[i] /= spectrum[i];
		imag[i] /= spectrum[i];
		spectrum[i] = a;
		real[i] *= spectrum[i];
		imag[i] *= spectrum[i];
	}
	if (i != 0 && i != timeSize / 2)
	{
		real[timeSize - i] = real[i];
		imag[timeSize - i] = -imag[i];
	}
}

void FFT::fft()
{
	for (int halfSize = 1; halfSize < realLength; halfSize *= 2)
	{
		// float k = -(float)Math.PI/halfSize;
		// phase shift step
		// float phaseShiftStepR = (float)Math.cos(k);
		// float phaseShiftStepI = (float)Math.sin(k);
		// using lookup table
		float phaseShiftStepR = fastcos(halfSize);
		float phaseShiftStepI = fastsin(halfSize);
		// current phase shift
		float currentPhaseShiftR = 1.0f;
		float currentPhaseShiftI = 0.0f;
		for (int fftStep = 0; fftStep < halfSize; fftStep++)
		{
			for (int i = fftStep; i < realLength; i += 2 * halfSize)
			{
				int off = i + halfSize;
				float tr = (currentPhaseShiftR * real[off]) - (currentPhaseShiftI * imag[off]);
				float ti = (currentPhaseShiftR * imag[off]) + (currentPhaseShiftI * real[off]);
				real[off] = real[i] - tr;
				imag[off] = imag[i] - ti;
				real[i] += tr;
				imag[i] += ti;
			}
			float tmpR = currentPhaseShiftR;
			currentPhaseShiftR = (tmpR * phaseShiftStepR) - (currentPhaseShiftI * phaseShiftStepI);
			currentPhaseShiftI = (tmpR * phaseShiftStepI) + (currentPhaseShiftI * phaseShiftStepR);
		}
	}
}

void FFT::init(ProcessorInitInfo initInfo)
{
	FourierTransform::init(initInfo);
	allocateArrays();
	if ((timeSize & (timeSize - 1)) != 0)
	{
		throw new std::logic_error("FFT: timeSize must be a power of two.");
	}
	buildReverseTable();
	buildTrigTables();
}

void FFT::forward(float * buffer, int bufferLength)
{
	if (bufferLength != timeSize)
	{
		printf("FFT.forward: The length of the passed sample buffer must be equal to timeSize().");
	}
	doWindow(buffer, bufferLength);

	bitReverseSamples(buffer, bufferLength, 0);

	fft();

	fillSpectrum();
}

void FFT::forward(float * buffer, int bufferLength, int startAt)
{
	if (bufferLength != timeSize)
	{
		printf("FFT.forward: The length of the passed sample buffer must be equal to timeSize().");
		return;
	}
	doWindow(buffer+startAt, bufferLength);

	bitReverseSamples(buffer, bufferLength, startAt);

	fft();

	fillSpectrum();
}

void FFT::forward(float * buffReal, int rLength, float * buffImag, int iLength)
{
	if (rLength != timeSize || iLength != timeSize)
	{
		printf("FFT.forward: The length of the passed buffers must be equal to timeSize().");
		return;
	}
	setComplex(buffReal, rLength, buffImag, iLength);
	bitReverseComplex();
	fft();
	fillSpectrum();
}

void FFT::buildReverseTable()
{
	int N = timeSize;
	reverse = new int[N];

	reverse[0] = 0;
	for (int limit = 1, bit = N / 2; limit < N; limit <<= 1, bit >>= 1)
	{
		for (int i = 0; i < limit; ++i)
		{
			reverse[i + limit] = reverse[i] + bit;
		}
	}
}

void FFT::bitReverseSamples(float * samples, int sampleSize, int startAt)
{
	for (int i = 0; i < timeSize; ++i)
	{
		real[i] = samples[startAt + reverse[i]];
		imag[i] = 0.0f;
	}
}

void FFT::bitReverseComplex()
{
	float* revReal = new float[realLength];
	float* revImag = new float[imagLength];
	for (int i = 0; i < realLength; i++)
	{
		revReal[i] = real[reverse[i]];
		revImag[i] = imag[reverse[i]];
	}
	delete real;
	delete imag;
	real = revReal;
	imag = revImag;
}

float FFT::fastsin(int i)
{
	return sinlookup[i];
}

float FFT::fastcos(int i)
{
	return coslookup[i];
}

void FFT::buildTrigTables()
{
	int N = timeSize;
	sinlookup = new float[N];
	coslookup = new float[N];
	for (int i = 0; i < N; ++i)
	{
		sinlookup[i] = (float)sin(-M_PI / (float)i);
		coslookup[i] = (float)cos(-M_PI / (float)i);
	}
}
