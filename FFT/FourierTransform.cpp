#include "FourierTransform.h"

const WindowFunction* FourierTransform::NONE = new RectangularWindow();
const WindowFunction* FourierTransform::HAMMING = new HammingWindow();
const WindowFunction* FourierTransform::HANN = new HannWindow();
const WindowFunction* FourierTransform::COSINE = new CosineWindow();
const WindowFunction* FourierTransform::TRIANGULAR = new TriangularWindow();
const WindowFunction* FourierTransform::BARTLETT = new BartlettWindow();
const WindowFunction* FourierTransform::BARTLETTHANN = new BartlettHannWindow();
const WindowFunction* FourierTransform::LANCZOS = new LanczosWindow();
const WindowFunction* FourierTransform::BLACKMAN = new BlackmanWindow();
const WindowFunction* FourierTransform::GAUSS = new GaussWindow();

FourierTransform::FourierTransform(int bufferSize, float sampleRate)
	: averagesLength{ 0 }
	, avgPerOctave{ 0 }
	, imagLength{ 0 }
	, realLength{ 0 }
	, spectrumLength{ 0 }
	, octaves{ 0 }
	, whichAverage{ NOAVG }
	, currentBufferIndex{ 0 }
{
	sampleBuffer.resize(static_cast<uint64_t>(bufferSize) * 4);
	timeSize = bufferSize;
	this->sampleRate = (int)sampleRate;
	bandWidth = (2.f / timeSize) * ((float)sampleRate / 2.f);
	noAverages();
	currentWindow = new RectangularWindow();
}

FourierTransform::~FourierTransform()
{
	if (real != nullptr) delete real;
	if (imag != nullptr) delete imag;
	if (spectrum != nullptr) delete spectrum;
	if (averages != nullptr) delete averages;
}

void FourierTransform::init()
{
}

void FourierTransform::setComplex(float* r, int rLength, float* i, int iLength)
{
	if (realLength != rLength && imagLength != iLength)
	{
		std::cerr << "FourierTransform.setComplex: the two arrays must be the same length as their member counterparts" << std::endl;
	}
	else
	{
		std::memcpy(real, r, rLength * sizeof(float));
		std::memcpy(imag, i, iLength * sizeof(float));
	}
}

void FourierTransform::fillSpectrum()
{
	for (int i = 0; i < spectrumLength; ++i)
	{
		spectrum[i] = (float)sqrt(real[i] * real[i] + imag[i] * imag[i]);
	}

	if (whichAverage == LINAVG)
	{
		int avgWidth = (int)spectrumLength / averagesLength;
		for (int i = 0; i < averagesLength; ++i)
		{
			float avg = 0;
			int j;
			for (j = 0; j < avgWidth; ++j)
			{
				int offset = j + i * avgWidth;
				if (offset < spectrumLength)
				{
					avg += spectrum[offset];
				}
				else
				{
					break;
				}
			}
			avg /= j + 1;
			averages[i] = avg;
		}
	}
	else if (whichAverage == LOGAVG)
	{
		for (int i = 0; i < octaves; ++i)
		{
			float lowFreq, hiFreq, freqStep;
			if (i == 0)
			{
				lowFreq = 0;
			}
			else
			{
				lowFreq = (sampleRate / 2) / (float)pow(2.f, octaves - i);
			}
			hiFreq = (sampleRate / 2) / (float)pow(2, octaves - i - 1);
			freqStep = (hiFreq - lowFreq) / avgPerOctave;
			float f = lowFreq;
			for (int j = 0; j < avgPerOctave; ++j)
			{
				int offset = j + i * avgPerOctave;
				averages[offset] = calcAvg(f, f + freqStep);
				f += freqStep;
			}
		}
	}
}

void FourierTransform::noAverages()
{
	if (averages != nullptr) delete averages;
	averages = new float[0];
	whichAverage = NOAVG;
}

void FourierTransform::linAverages(int numAvg)
{
	if (numAvg > spectrumLength / 2)
	{
		std::cerr << "The number of averages for this transform can be at most "
			<< spectrumLength / 2 << "." << std::endl;
	}
	else
	{
		if (averages != nullptr) delete averages;
		averages = new float[numAvg];
	}
	whichAverage = LINAVG;
}

void FourierTransform::logAverages(int minBandwidth, int bandsPerOctave)
{
	float nyq = (float)sampleRate / 2.f;
	octaves = 1;
	while ((nyq /= 2) > minBandwidth)
	{
		octaves++;
	}
	std::cout << "Number of octaves: " << octaves << std::endl;
	avgPerOctave = bandsPerOctave;
	if (averages != nullptr) delete averages;
	averages = new float[static_cast<uint64_t>(octaves) * bandsPerOctave];
	whichAverage = LOGAVG;
}

void FourierTransform::window(WindowFunction * windowFunction)
{
	this->currentWindow = windowFunction;
}

void FourierTransform::doWindow(float* samples, int length)
{
	currentWindow->apply(samples, length);
}

int FourierTransform::getTimeSize()
{
	return timeSize;
}

int FourierTransform::getSpecSize()
{
	return spectrumLength;
}

float FourierTransform::getBand(int i)
{
	if (i < 0) i = 0;
	if (i > spectrumLength - 1) i = spectrumLength - 1;
	return spectrum[i];
}

float FourierTransform::getBandWidth()
{
	return bandWidth;
}

float FourierTransform::getAverageBandWidth(int averageIndex)
{
	if (whichAverage == LINAVG)
	{
		int avgWidth = (int)spectrumLength / averagesLength;
		return avgWidth * getBandWidth();
	}
	else if (whichAverage == LOGAVG)
	{
		int octave = averageIndex / avgPerOctave;
		float lowFreq, hiFreq, freqStep;

		if (octave == 0)
		{
			lowFreq = 0;
		}
		else
		{
			lowFreq = (sampleRate / 2) / (float)pow(2, octaves - octave);
		}

		hiFreq = (sampleRate / 2) / (float)pow(2, octaves - octave - 1);

		freqStep = (hiFreq - lowFreq) / avgPerOctave;

		return freqStep;
	}
	return 0.f;
}

int FourierTransform::freqToIndex(float freq)
{
	if (freq < getBandWidth() / 2) return 0;
	if (freq > sampleRate / 2 - getBandWidth() / 2) return spectrumLength - 1;
	float fraction = freq / (float)sampleRate;
	int i = round(timeSize * fraction);
	return i;
}

float FourierTransform::indexToFreq(int i)
{
	float bw = getBandWidth();

	if (i == 0)return bw * 0.25f;

	if (i == spectrumLength - 1)
	{
		float lastBinBeginFreq = (sampleRate / 2) - (bw / 2);
		float binHalfWidth = bw * 0.25f;
		return lastBinBeginFreq + binHalfWidth;
	}
	return i * bw;
}

float FourierTransform::getAverageCenterFrequency(int i)
{
	if (whichAverage == LINAVG)
	{
		int avgWidth = (int)spectrumLength / averagesLength;

		int centerBinIndex = i * avgWidth + avgWidth / 2;
		return indexToFreq(centerBinIndex);
	}
	else if (whichAverage == LOGAVG)
	{
		int octave = i / avgPerOctave;

		int offset = i % avgPerOctave;
		float lowFreq, hiFreq, freqStep;

		if (octave == 0)
		{
			lowFreq = 0;
		}
		else
		{
			lowFreq = (sampleRate / 2) / (float)pow(2, octaves - octave);
		}

		hiFreq = (sampleRate / 2) / (float)pow(2, octaves - octave - 1);

		freqStep = (hiFreq - lowFreq) / avgPerOctave;

		float f = lowFreq + offset * freqStep;

		return f + freqStep / 2;
	}
	return 0;
}

float FourierTransform::getFreq(float freq)
{
	return getBand(freqToIndex(freq));
}

void FourierTransform::setFreq(float freq, float a)
{
	setBand(freqToIndex(freq), a);
}

void FourierTransform::scaleFreq(float freq, float s)
{
	scaleBand(freqToIndex(freq), s);
}

int FourierTransform::getAvgSize()
{
	return averagesLength;
}

float FourierTransform::getAvg(int i)
{
	float ret;
	if (averagesLength > 0)
		ret = averages[i];
	else
		ret = 0;
	return ret;
}

float FourierTransform::calcAvg(float lowFreq, float hiFreq)
{
	int lowBound = freqToIndex(lowFreq);
	int hiBound = freqToIndex(hiFreq);
	float avg = 0;
	for (int i = lowBound; i <= hiBound; i++)
	{
		avg += spectrum[i];
	}
	avg /= (hiBound - lowBound + 1);
	return avg;
}

float* FourierTransform::getSpectrumReal()
{
	return real;
}

float* FourierTransform::getSpectrumImaginary()
{
	return imag;
}

void FourierTransform::finalizeConverterInfo(ConverterInitInfo& initInfo)
{
	initInfo.outChannelCount = 1;
	initInfo.outChannelLayout = AV_CH_LAYOUT_MONO;
	initInfo.outSampleFormat = AV_SAMPLE_FMT_FLT;
}

void FourierTransform::forward(const ConvertedSampleContainer& container)
{
	float* samplePointer = &sampleBuffer[currentBufferIndex];
	std::memcpy(samplePointer, container.convertedSamples, container.dataSize);
	currentBufferIndex += container.numSamples;
	if (currentBufferIndex > timeSize)
	{
		forward(sampleBuffer.data(), timeSize);
		uint64_t overflow = (uint64_t)currentBufferIndex - timeSize;
		std::memcpy(sampleBuffer.data(), sampleBuffer.data() + timeSize, overflow * sizeof(float));
	}
}

void FourierTransform::forward(float* buffer, int bufferLength, int startAt)
{
	if (bufferLength - startAt < timeSize)
	{
		std::cerr << "FourierTransform.forward: not enough samples in the buffer between " +
			startAt << " and " << bufferLength << " to perform a transform." << std::endl;
		return;
	}

	float* section = new float[timeSize];
	std::memcpy(section, buffer + startAt, sizeof(float) * timeSize);
	forward(section, timeSize);
}

