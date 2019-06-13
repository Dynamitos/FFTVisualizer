#include "Resources.h"
#include "AudioFileReader.h"
#include "FFT.h"
#include "SampleConverter.h"
#include "sdlutil.h"
#include "OpenALPlayer.h"
#include "WindowsPlayer.h"
#include <thread>

#undef main
int main()
{
	//Audio input
	AudioSourceInfo sourceInfo;
	sourceInfo.url = "Africa.mp3";
	AudioInput* audioInput = new AudioFileReader();
	audioInput->setStreamSource(sourceInfo);
	
	//Player
	PlayerInitInfo playerInitInfo;
	playerInitInfo.numBuffers = 3;
	AudioPlayer* audioPlayer = new OpenALPlayer();
	audioPlayer->init(playerInitInfo);
	
	//FFT
	//change to use ProcessorInfo
	ProcessorInitInfo fftInfo;// = commandline.getfftInfo
	fftInfo.fftSize = 4096;
	AudioProcessor* fft = new FFT(fftInfo.fftSize, audioInput->getSampleRate());
	
	//Visualizer
	Display* display = new Display(1920, 1080);

	//Input to player
	ConverterInitInfo inputToPlayerInfo;
	audioInput->fillConverterInfo(inputToPlayerInfo);
	audioPlayer->finalizeConverterInfo(inputToPlayerInfo);
	SampleConverter* inputToPlayerConverter = new SampleConverter(inputToPlayerInfo);

	//input to FFT
	ConverterInitInfo inputToProcessorInfo;
	audioInput->fillConverterInfo(inputToProcessorInfo);
	fft->finalizeConverterInfo(inputToProcessorInfo);
	SampleConverter* inputToProcessorConverter = new SampleConverter(inputToProcessorInfo);
	
	//File to player converter
	PacketQueue<AudioInputContainer>* inputQueue = new PacketQueue<AudioInputContainer>();
	//player converter to player
	PacketQueue<SampleContainer>* outputQueue = new PacketQueue<SampleContainer>();
	//File to FFT converter
	PacketQueue<AudioInputContainer>* fftInputQueue = new PacketQueue<AudioInputContainer>();
	//FFT converter to FFT Processor
	PacketQueue<SampleContainer>* fftOutputQueue = new PacketQueue<SampleContainer>();
	//FFT Processor to visualizer
	PacketQueue<SampleContainer>* fftProcessedQueue = new PacketQueue<SampleContainer>();

	audioInput->registerPacketQueue(inputQueue);
	audioInput->registerPacketQueue(fftInputQueue);

	std::thread inputThread(&AudioInput::run, audioInput);
	std::thread converterThread(&SampleConverter::run, inputToPlayerConverter, inputQueue, outputQueue);
	std::thread playerThread(&AudioPlayer::run, audioPlayer, outputQueue);
	std::thread fftConverterThread(&SampleConverter::run, inputToProcessorConverter, fftInputQueue, fftOutputQueue);
	std::thread fftThread(&AudioProcessor::run, fft, fftOutputQueue, fftProcessedQueue);
	std::thread visualizerThread(&Display::run, display, fftProcessedQueue);

	inputThread.join();
	converterThread.join();
	playerThread.join();
	fftConverterThread.join();
	fftThread.join();
	visualizerThread.join();
}