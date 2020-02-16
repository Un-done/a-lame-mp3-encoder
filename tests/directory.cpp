#include <gtest/gtest.h>
#include "directory.h"

#ifndef LAME_ENC_TEST_DATA_DIR
#define LAME_ENC_TEST_DATA_DIR "tests/test_data"
#endif

using namespace std::literals::string_literals;

TEST(lame_enc, directory) {
    std::vector<std::string> expected = {
      LAME_ENC_TEST_DATA_DIR "/.gitignore",
      LAME_ENC_TEST_DATA_DIR "/empty_dir",
      LAME_ENC_TEST_DATA_DIR "/sound.wav",
      LAME_ENC_TEST_DATA_DIR "/sound1.wav",
      LAME_ENC_TEST_DATA_DIR "/sound2.wav"};

    ASSERT_EQ(true,true);
    std::vector<std::string> actual = vscharf::directory_entries(std::string(LAME_ENC_TEST_DATA_DIR));
    std::sort(begin(actual), end(actual));
    ASSERT_EQ(expected, actual);
}

TEST(lame_enc, directory_empty) {
    // assumes the test is called in the project root directory
    std::vector<std::string> expected = { };

    std::vector<std::string> actual = vscharf::directory_entries(std::string(LAME_ENC_TEST_DATA_DIR "/empty_dir"));
    std::sort(begin(actual), end(actual));
    ASSERT_EQ(expected, actual);

}

TEST(lame_enc, directory_invalid) {
  {
    ASSERT_THROW(vscharf::directory_entries("non_existent_dir"s), std::exception);
  }
}
