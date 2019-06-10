#pragma once
#include "AudioPlayer.h"
#include <vector>
#include <mmdeviceapi.h>
#include <Audioclient.h>

class WindowsPlayer : public AudioPlayer
{
public:
	WindowsPlayer();
	virtual ~WindowsPlayer();
	// Inherited via AudioPlayer
	virtual void init(PlayerInitInfo initInfo) override;
	virtual void finalizeConverterInfo(ConverterInitInfo& converterInfo) override;
	virtual void playSamples(std::unique_ptr<ConvertedSampleContainer> container) override;
private:
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioClient* pAudioClient = NULL;
	IAudioRenderClient* pRenderClient = NULL;
	WAVEFORMATEX* pwfx = NULL;
	uint32_t bufferFrameCount;
	uint32_t numFramesPadding;
	uint8_t* data;
	std::vector<uint8_t> surplusFramebuffer;
	REFERENCE_TIME hnsActualDuration;
	bool initBuffer = true;
};
