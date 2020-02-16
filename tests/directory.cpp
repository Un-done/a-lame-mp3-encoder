#include <gtest/gtest.h>
#include "directory.h"

TEST(lame_enc, directory) {
#ifdef WINDOWS
    std::vector<std::string> expected = { "test_data\\.gitignore", "test_data\\empty_dir",
					  "test_data\\sound.wav", "test_data\\sound1.wav",
					  "test_data\\sound2.wav" };
#else
    std::vector<std::string> expected = {
      "tests/test_data/.", "tests/test_data/..", "tests/test_data/.gitignore", "tests/test_data/empty_dir",
      "tests/test_data/sound.wav", "tests/test_data/sound1.wav", "tests/test_data/sound2.wav"};
#endif
    ASSERT_EQ(true,true);
    std::vector<std::string> actual = vscharf::directory_entries("tests/test_data");
    std::sort(begin(actual), end(actual));
    ASSERT_EQ(expected, actual);
}

TEST(lame_enc, directory_empty) {
    // assumes the test is called in the project root directory
#ifdef WINDOWS
    std::vector<std::string> expected = {};
#else
    std::vector<std::string> expected = {
      "tests/test_data/empty_dir/.", "tests/test_data/empty_dir/.." };
#endif

#ifdef WINDOWS
    std::vector<std::string> actual = vscharf::directory_entries("test_data\\empty_dir");
#else
    std::vector<std::string> actual = vscharf::directory_entries("tests/test_data/empty_dir");
#endif
    std::sort(begin(actual), end(actual));
    ASSERT_EQ(expected, actual);

}

TEST(lame_enc, directory_invalid) {
  {
    ASSERT_THROW(vscharf::directory_entries("non_existent_dir"), std::exception);
  }
}
