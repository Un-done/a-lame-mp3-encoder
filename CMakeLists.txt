cmake_minimum_required(VERSION 3.12)
project (a-lame-mp3-encoder)

#option(LAME_TESTS "build tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
find_package(Lame REQUIRED)
find_package(Filesystem REQUIRED Final)
#find_package(ext-basics REQUIRED)


add_library(lameEnc STATIC
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/wavdecoder.cpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/directory.cpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/mp3encoder.cpp
)
target_link_libraries(lameEnc PUBLIC
    #ext::basics
    std::filesystem
    Lame::Lame
    Threads::Threads
)
target_include_directories(lameEnc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# build binary
add_executable(a-lame-mp3-encoder
		 ${CMAKE_CURRENT_SOURCE_DIR}/src/batch-encoder.cpp
)
target_link_libraries(a-lame-mp3-encoder PRIVATE lameEnc)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external_libraries/googletest/CMakeLists.txt)
    include(CTest)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external_libraries/googletest EXCLUDE_FROM_ALL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()
