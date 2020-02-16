#include "wavdecoder.h"
#include <gtest/gtest.h>

#include <fstream>

TEST(lame_enc, wavdecoder) {
    std::ifstream input_file("tests/test_data/sound.wav");

    vscharf::WavDecoder w(input_file);
    ASSERT_EQ(w.get_header().channels, 1);
    ASSERT_EQ(w.get_header().samplesPerSec, 44100);
    ASSERT_EQ(w.get_header().avgBytesPerSec, 0x15888);
    ASSERT_EQ(w.get_header().blockAlign, 0x2);
    ASSERT_EQ(w.get_header().bitsPerSample, 16);

    std::size_t nsamples = 0;

    const auto& buf = w.read_samples(1);
    ++nsamples;
    while (w.has_next()) {
        const auto& buf = w.read_samples(10);
        if (w.has_next()) {
            assert(buf.size() == 10);
        }
        nsamples += buf.size();
    }

    ASSERT_EQ(nsamples, 0x10266 / 2);
}
