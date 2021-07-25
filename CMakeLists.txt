cmake_minimum_required(VERSION 3.12)
project (a-lame-mp3-encoder)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external_libraries/lame)

add_library(lameEnc STATIC
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/wavdecoder.cpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/directory.cpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/mp3encoder.cpp
)
target_link_libraries(lameEnc PUBLIC
    mp3lame
)
target_compile_features(lameEnc PUBLIC cxx_std_17)
target_include_directories(lameEnc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# build binary
add_executable(a-lame-mp3-encoder
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/batch-encoder.cpp
)

target_compile_features(a-lame-mp3-encoder PRIVATE cxx_std_17)
target_link_libraries(a-lame-mp3-encoder PRIVATE lameEnc)
