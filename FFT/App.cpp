#include "Utils/Resources.h"
#include "Input/AudioFileReader.h"
#include "Processing/FFT.h"
#include "Utils/SampleConverter.h"
#include "Visualization/OpenGL/Renderer.h"
#include "Player/OpenALPlayer.h"
#include "Player/WindowsPlayer.h"
#include "Processing/FFTFilter.h"
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
	visualizerInfo.imageURL = "Wallpaper.jpg";
	visualizerInfo.intensityOffset = 0.5f;
	visualizerInfo.intensityScale = 2.0f;
	visualizerInfo.name = "Test";
	visualizerInfo.overlayURL = "Overlay.png";
	visualizerInfo.scaling = false;
	visualizerInfo.screenDimensions = glm::ivec2(1280, 720);
	visualizerInfo.vSync = true;
	visualizerInfo.numBands = fftInfo.numBands;
	visualizerInfo.tesselationLevel = fftInfo.tesselationLevel;
	visualizerInfo.line.start = glm::vec2(-0.490625f, -0.6648148f);
	visualizerInfo.line.end = glm::vec2(0.49270832f, -0.6648148f);
	visualizerInfo.line.lineColor = glm::vec4(0, 1, 1, 1);
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
	//Visualizer is potentially OpenGL, which only runs on the MAIN thread
	visualizer->run(fftFilteredQueue);

	inputThread.join();
	converterThread.join();
	playerThread.join();
	fftConverterThread.join();
	fftFilterThread.join();
	fftThread.join();

	delete audioInput;
	delete audioPlayer;
	delete fft;
	delete fftFilter;
	delete visualizer;
	delete inputToPlayerConverter;
	delete inputToProcessorConverter;
	delete inputQueue;
	delete outputQueue;
	delete fftInputQueue;
	delete fftOutputQueue;
	delete fftProcessedQueue;
	delete fftFilteredQueue;
}