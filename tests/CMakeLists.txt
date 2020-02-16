project(lameEnc-tests)

set(test_sources 
    "directory.cpp"
    "mp3encoder.cpp"
    "wavdecoder.cpp"
)

set(test_target test-lameEnc)
add_executable("${test_target}" gtest.cpp ${test_sources})
target_include_directories("${test_target}" SYSTEM PRIVATE ${gtest_SOURCE_DIR}/include)
target_link_libraries("${test_target}"
    gtest_main gtest
    lameEnc
)

add_test(NAME "${test_target}_run" COMMAND $<TARGET_FILE:${test_target}>)
set_target_properties (${test_target} PROPERTIES FOLDER tests/${test_target})
