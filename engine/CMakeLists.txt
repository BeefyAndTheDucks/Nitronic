file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)
file(GLOB_RECURSE ENGINE_HEADERS include/*.h)
add_library(engine STATIC ${ENGINE_HEADERS})
target_sources(engine PRIVATE ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC include)

# link external deps here