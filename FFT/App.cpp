#include "Resources.h"
#include "AudioFileReader.h"
#include "FFT.h"
#include "SampleConverter.h"
#include "Visualizer.h"
#include "OpenALPlayer.h"
#include "WindowsPlayer.h"
#include "FFTFilter.h"
#include <thread>

#undef main
int main()
{
	//Audio input
	AudioSourceInfo sourceInfo;
	sourceInfo.url = "Obsessed.mp3";
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
	fftInfo.fftSize = 2048;
	fftInfo.sampleRate = audioInput->getSampleRate();
	fftInfo.numBands = 32;
	fftInfo.tesselationLevel = 64;
	AudioProcessor* fft = new FFT();
	fft->init(fftInfo);
	AudioProcessor* fftFilter = new FFTFilter();
	fftFilter->init(fftInfo);
	
	//Visualizer
	AudioVisualizerInfo visualizerInfo;
	visualizerInfo.imageURL = "Test.png";
	visualizerInfo.intensityOffset = 0.5f;
	visualizerInfo.intensityScale = 2.0f;
	visualizerInfo.name = "Test";
	visualizerInfo.overlayURL = "Overlay.png";
	visualizerInfo.scaling = true;
	visualizerInfo.screenDimensions = glm::ivec2(1920, 1080);
	visualizerInfo.vSync = true;
	AudioVisualizer* visualizer = new GL::Renderer();
	visualizer->init(visualizerInfo);

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
	//FFT Processor to filter
	PacketQueue<SampleContainer>* fftProcessedQueue = new PacketQueue<SampleContainer>();
	//FFT Filter to visualizer
	PacketQueue<SampleContainer>* fftFilteredQueue = new PacketQueue<SampleContainer>();

	audioInput->registerPacketQueue(inputQueue);
	audioInput->registerPacketQueue(fftInputQueue);

	std::thread inputThread(&AudioInput::run, audioInput);
	std::thread converterThread(&SampleConverter::run, inputToPlayerConverter, inputQueue, outputQueue);
	std::thread playerThread(&AudioPlayer::run, audioPlayer, outputQueue);
	std::thread fftConverterThread(&SampleConverter::run, inputToProcessorConverter, fftInputQueue, fftOutputQueue);
	std::thread fftThread(&AudioProcessor::run, fft, fftOutputQueue, fftProcessedQueue);
	std::thread fftFilterThread(&AudioProcessor::run, fftFilter, fftProcessedQueue, fftFilteredQueue);
	std::thread visualizerThread(&AudioVisualizer::run, visualizer, fftFilteredQueue);

	inputThread.join();
	converterThread.join();
	playerThread.join();
	fftConverterThread.join();
	fftFilterThread.join();
	fftThread.join();
	visualizerThread.join();
}