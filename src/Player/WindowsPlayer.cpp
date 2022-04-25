#include "WindowsPlayer.h"

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

WindowsPlayer::WindowsPlayer()
{
}

WindowsPlayer::~WindowsPlayer()
{
}

void WindowsPlayer::init(PlayerInitInfo initInfo)
{
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)& pEnumerator);

	hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDevice);

	hr = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)& pAudioClient);

	hr = pAudioClient->GetMixFormat(&pwfx);

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		0,
		hnsRequestedDuration,
		0,
		pwfx,
		NULL);

	hr = pAudioClient->GetBufferSize(&bufferFrameCount);

	hr = pAudioClient->GetService(
		IID_IAudioRenderClient,
		(void**)& pRenderClient);
}

void WindowsPlayer::finalizeConverterInfo(ConverterInitInfo& converterInfo)
{
	converterInfo.outSampleRate = pwfx->nSamplesPerSec;
	if (converterInfo.inChannelCount != pwfx->nChannels)
	{
		printf("Fuck you Microsoft");
	}
	converterInfo.outChannelCount = pwfx->nChannels;
	switch (pwfx->wBitsPerSample)
	{
	case 8:
		converterInfo.outSampleFormat = AV_SAMPLE_FMT_U8;
		break;
	case 16:
		converterInfo.outSampleFormat = AV_SAMPLE_FMT_S16;
		break;
	case 32:
		converterInfo.outSampleFormat = AV_SAMPLE_FMT_S32;
		break;
	default:
		printf("Fuck you Microsoft");
	}
}

void WindowsPlayer::playSamples(std::unique_ptr<SampleContainer> container)
{
	if (initBuffer)
	{
		pRenderClient->GetBuffer(bufferFrameCount, &data);
		std::memcpy(data, container->convertedSamples, container->dataSize);
		pRenderClient->ReleaseBuffer(bufferFrameCount, 0);
		hnsActualDuration = REFTIMES_PER_SEC *
			bufferFrameCount / pwfx->nSamplesPerSec;
		pAudioClient->Start();
		initBuffer = false;
	}
	else
	{
		Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

		pAudioClient->GetCurrentPadding(&numFramesPadding);
		//find out how much space is left in the audio buffer
		uint32_t numFramesAvailable = bufferFrameCount - numFramesPadding;
		uint32_t numBytesAvailable = numFramesAvailable * pwfx->wBitsPerSample / 8;
		uint32_t totalBytesWritten = 0;

		pRenderClient->GetBuffer(numFramesAvailable, &data);
		if (surplusFramebuffer.size() > 0)
		{
			//load surplus data from the last call into the buffer
			std::memcpy(data, surplusFramebuffer.data(), surplusFramebuffer.size());
			numBytesAvailable -= (uint32_t)surplusFramebuffer.size();

			totalBytesWritten = static_cast<uint32_t>(surplusFramebuffer.size());
			surplusFramebuffer.clear();
		}
		uint32_t numBytesWritten = min(numBytesAvailable, container->dataSize);
		std::memcpy(data + totalBytesWritten, container->convertedSamples, numBytesWritten);
		totalBytesWritten += numBytesWritten;
		if (numBytesWritten < container->dataSize)
		{
			//calculates how many more frames were passed to this function than fit into the audio buffer
			uint32_t surplusBytes = container->dataSize - numBytesWritten;

			//copy the surplus data into the 
			surplusFramebuffer.resize(surplusBytes);
			std::memcpy(surplusFramebuffer.data(), container->convertedSamples + numBytesWritten, container->dataSize - numBytesWritten);
		}
	}
}
