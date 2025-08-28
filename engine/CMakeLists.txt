file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)
add_library(engine STATIC)
target_sources(engine PRIVATE ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC include)

target_link_libraries(engine PUBLIC glfw)