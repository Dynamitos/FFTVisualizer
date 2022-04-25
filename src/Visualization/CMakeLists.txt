add_subdirectory(OpenGL/)

target_sources(Visualizer
    PRIVATE
        AudioVisualizer.h
        AudioVisualizer.cpp
        RandomNoise.h
        RandomNoise.cpp
        sdlutil.h
        sdlutil.cpp)