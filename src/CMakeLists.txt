add_subdirectory(Input/)
add_subdirectory(Player/)
add_subdirectory(Processing/)
add_subdirectory(Utils/)
add_subdirectory(Visualization/)

target_sources(Visualizer
    PRIVATE
        App.cpp)