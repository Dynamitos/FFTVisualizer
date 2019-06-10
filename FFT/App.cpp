#include "Resources.h"
#include "AudioFileReader.h"
#include "FFT.h"
#include "SampleConverter.h"
#include "sdlutil.h"
#include "OpenALPlayer.h"
#include "WindowsPlayer.h"

#undef main
int main()
{
	ProcessorInitInfo fftInfo;// = commandline.getfftInfo
	fftInfo.fftSize = 4096;
	AudioSourceInfo sourceInfo;
	sourceInfo.url = "Obsessed.mp3";
	AudioInput* audioInput = new AudioFileReader();
	audioInput->setStreamSource(sourceInfo);
	PlayerInitInfo playerInitInfo;
	playerInitInfo.numBuffers = 3;
	AudioPlayer* audioPlayer = new OpenALPlayer();
	audioPlayer->init(playerInitInfo);
	//change to use ProcessorInfo
	AudioProcessor* fft = new FFT(fftInfo.fftSize, audioInput->getSampleRate());
	ConverterInitInfo inputToPlayerInfo;
	audioInput->fillConverterInfo(inputToPlayerInfo);
	audioPlayer->finalizeConverterInfo(inputToPlayerInfo);
	SampleConverter* inputToPlayerConverter = new SampleConverter(inputToPlayerInfo);

	ConverterInitInfo inputToProcessorInfo;
	audioInput->fillConverterInfo(inputToProcessorInfo);
	fft->finalizeConverterInfo(inputToProcessorInfo);
	SampleConverter* inputToProcessorConverter = new SampleConverter(inputToProcessorInfo);
	
	PacketQueue<AudioInputContainer>* inputQueue = new PacketQueue<AudioInputContainer>();
	PacketQueue<ConvertedSampleContainer>* outputQueue = new PacketQueue<ConvertedSampleContainer>();

	std::thread inputThread(&AudioInput::run, audioInput, inputQueue);
	std::thread converterThread(&SampleConverter::run, inputToPlayerConverter, inputQueue, outputQueue);
	std::thread playerThread(&AudioPlayer::run, audioPlayer, outputQueue);
	converterThread.join();
	inputThread.join();
	playerThread.join();
}