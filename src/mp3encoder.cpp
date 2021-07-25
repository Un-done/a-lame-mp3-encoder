// This struct implements RAII for single instance of a lame encoder.
#include "mp3encoder.h"

#include "wavdecoder.h"
#include <ostream>

using namespace vscharf;

Mp3Encoder::Mp3Encoder(int quality) : gfp_(lame_init()), quality_(quality) {
    if (!gfp_) {
        throw lame_error("Call to lame_init() failed!");
    }
}

Mp3Encoder::~Mp3Encoder() {
    lame_close(gfp_);
}

// Encode the data from in to an ostream out taking nsamples at
// once. If nsamples is zero it will be chosen such that 4k bytes will
// be processed at once.
void Mp3Encoder::encode(WavDecoder& in, std::ostream& out, uint32_t nsamples /* = 0 */) {
    if (!out) {
        throw decoder_error("Invalid output stream!");
    }

    // initialize lame
    lame_set_num_channels(gfp_, in.get_header().channels);
    lame_set_in_samplerate(gfp_, in.get_header().samplesPerSec);
    lame_set_quality(gfp_, quality_);
    if (lame_init_params(gfp_) < 0) {
        throw lame_error("lame initialization failed!");
    }

    // auto-determine sample size
    if (!nsamples) {
        nsamples = 4096 / in.get_header().bytesPerSample;
    }
    std::vector<char> buf(1.25 * nsamples + 7200); // worst-case estimate from lame/API

    // the actual encoding
    while (in.has_next()) {
        const auto& inbuf = in.read_samples(nsamples);

        int n;
        if (in.get_header().channels > 1) {
            n = lame_encode_buffer_interleaved(gfp_,
                                               const_cast<int16_t*>(&inbuf[0]),
                                               inbuf.size() / in.get_header().channels,
                                               reinterpret_cast<unsigned char*>(buf.data()),
                                               buf.size());
        } else {
            n = lame_encode_buffer(gfp_,
                                   const_cast<int16_t*>(&inbuf[0]),
                                   nullptr,
                                   inbuf.size(),
                                   reinterpret_cast<unsigned char*>(buf.data()),
                                   buf.size());
        }
        if (n < 0) {
            throw decoder_error("lame_encode_buffer returned error!");
        }
        if (!out.write(buf.data(), n)) {
            throw decoder_error("Writing to output failed!");
        }
    }

    //flush the rest
    buf.resize(7200);
    auto n = lame_encode_flush(gfp_, reinterpret_cast<unsigned char*>(buf.data()), buf.size());
    if (n < 0) {
        throw decoder_error("lame_encode_flush returned error!");
    } else if (n > 0 && !out.write(buf.data(), n)) {
        throw decoder_error("Writing to output failed!");
    }
}
