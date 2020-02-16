# - Try to find Lame
# Once done this will define
#
#  LAME_FOUND - system has Lame
#  LAME_INCLUDE_DIR - the Lame include directory
#  LAME_LIBRARIES - Link these to use Lame
#  LAME_DEFINITIONS - Compiler switches required for using Lame

if (TARGET Lame::Lame)
    return()
endif()

find_path(LAME_INCLUDE_DIR NAMES lame/lame.h)
find_library(LAME_LIBRARIES NAMES libmp3lame.a mp3lame)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lame DEFAULT_MSG LAME_INCLUDE_DIR LAME_LIBRARIES )

# show the LAME_INCLUDE_DIR and LAME_LIBRARIES variables only in the advanced view
mark_as_advanced(LAME_INCLUDE_DIR LAME_LIBRARIES)

if (${LAME_FOUND})
    add_library(Lame::Lame INTERFACE IMPORTED)
    target_include_directories(Lame::Lame INTERFACE ${LAME_INCLUDE_DIR})
    target_link_libraries(Lame::Lame INTERFACE ${LAME_LIBRARIES})
    target_compile_definitions(Lame::Lame INTERFACE ${LAME_DEFINITIONS})
endif()
