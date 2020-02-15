cmake_minimum_required(VERSION 3.12)
project (a-lame-mp3-encoder)

option(LAME_TESTS "build tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
find_package(Lame REQUIRED)

# build binary
add_executable(a-lame-mp3-encoder
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/wavdecoder.cpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/directory.cpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/mp3encoder.cpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/batch-encoder.cpp
)
target_link_libraries(a-lame-mp3-encoder PRIVATE Lame::Lame Threads::Threads)
target_include_directories(a-lame-mp3-encoder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


# build tests
if(${LAME_TESTS})
add_executable(dir_test ${CMAKE_CURRENT_SOURCE_DIR}/src/directory.cpp)
target_compile_definitions(dir_test PRIVATE TEST_DIR)
target_include_directories(dir_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(wav_test ${CMAKE_CURRENT_SOURCE_DIR}/src/wavdecoder.cpp)
target_compile_definitions(wav_test PRIVATE TEST_WAV)
target_include_directories(wav_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(enc_test
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mp3encoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wavdecoder.cpp
)
target_compile_definitions(enc_test PRIVATE TEST_ENC)
target_link_libraries(enc_test PRIVATE Lame::Lame)
target_include_directories(enc_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()
