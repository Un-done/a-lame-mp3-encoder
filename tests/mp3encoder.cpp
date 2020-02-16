#include "mp3encoder.h"
#include "wavdecoder.h"
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <sstream>

TEST(lame_enc, mp3encoder) {

    std::string input("tests/test_data/sound.wav");

    std::ifstream wav_file(input);
    std::stringstream output;

    try {
        vscharf::WavDecoder w(wav_file);
        vscharf::Mp3Encoder l(2);
        l.encode(w, output);
    } catch (const std::runtime_error& d) {
        std::cerr << std::endl << "Error decoding test_data/sound.wav: " << d.what() << std::endl;
        throw;
    }

    std::string bytes(4, 0);

    // header of mp3 file
    output.read(&bytes[0], 4);
    ASSERT_EQ(bytes, (std::string{'\xff', '\xfb', '\x50', '\xc4'}));

    // somewhere in the middle
    output.ignore(0x300); // seek 0x304 bytes into the file
    output.read(&bytes[0], 4);
    ASSERT_EQ(bytes, (std::string{'\xfa', '\xbd', '\x5f', '\x51'}));

    // (almost) last bytes
    output.ignore(0x1428); // seek 0x1730 bytes into the file
    output.read(&bytes[0], 4);
    ASSERT_EQ(bytes, (std::string{'\x0d', '\x5f', '\xed', '\x4a'}));
}
