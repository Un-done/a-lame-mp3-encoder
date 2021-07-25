#include "wavdecoder.h"

#include <algorithm> // generate_n
#include <exception> // terminate
#include <iterator>  // istream_iterator
#include <string>
#include <utility> // swap

namespace vscharf {

// constexpr bool little_endian()
// {
//   return ((char*)endiadness)[0] == 0xDD;
// }

// ======== helper functions ========
namespace {

constexpr static uint32_t endiadness{0xAABBCCDD};

inline bool is_little_endian() {
    return ((unsigned char*) &endiadness)[0] == 0xDD;
}

template<typename Result>
inline Result read_integral(std::istream& in) {
    static_assert(std::is_integral<Result>::value, "only integral types");
    static thread_local std::string bytes(sizeof(Result), 0);
    in.read(&bytes[0], sizeof(Result)); // explictely allowed since C++11
    if (!is_little_endian()) {
        switch (sizeof(Result)) {
            case 1:
                break;
            case 2:
                std::swap(bytes[0], bytes[1]);
                break;
            case 4:
                std::swap(bytes[0], bytes[3]);
                std::swap(bytes[1], bytes[2]);
                break;
            case 8:
                std::swap(bytes[0], bytes[7]);
                std::swap(bytes[1], bytes[6]);
                std::swap(bytes[2], bytes[5]);
                std::swap(bytes[3], bytes[4]);
                break;
            default:
                std::terminate();
        }
    }
    return *reinterpret_cast<const Result*>(bytes.data());
} // read_integral

template<std::size_t Width>
inline std::string read_string(std::istream& in) {
    std::string bytes(Width, 0);
    in.read(&bytes[0], Width); // explictely allowed since C++11
    return bytes;
} // read_string

} // anonymous namespace

// Constructs a WavDecoder object, fills the WavHeader and seeks to
// the first data chunk.
WavDecoder::WavDecoder(std::istream& in) : in_(in) {
    if (!in_) {
        throw decoder_error("Couldn't open file!");
    }
    decode_wav_header();
}

// Skips the next chunk of the wave file. Assumes that the ckID has
// already been read and the next 4 bytes contain the chunk size.
void WavDecoder::skip_chunk() {
    auto chunk_size = read_integral<uint32_t>(in_);
    in_.ignore(chunk_size);
}

// Decodes the RIFF/WAVE header and fills the information into
// header_.  File Specification taken from
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
void WavDecoder::decode_wav_header() {
    if (read_string<4>(in_) != "RIFF") {
        throw decoder_error("No RIFF file");
    }
    uint32_t in_size = read_integral<uint32_t>(in_);
    if (read_string<4>(in_) != "WAVE") {
        throw decoder_error("No WAVE type");
    }

    while (in_.good() && read_string<4>(in_) != "fmt ") skip_chunk();
    if (!in_.good()) {
        throw decoder_error("No format chunk");
    }

    auto fmt_chunk_size = read_integral<uint32_t>(in_);

    if (read_integral<uint16_t>(in_) != 0x1) {
        throw decoder_error("No PCM format");
    }
    header_.channels = read_integral<uint16_t>(in_);
    header_.samplesPerSec = read_integral<uint32_t>(in_);
    header_.avgBytesPerSec = read_integral<uint32_t>(in_);
    header_.blockAlign = read_integral<uint16_t>(in_);
    header_.bitsPerSample = read_integral<uint16_t>(in_);

    // sample size is M-byte with M = block-align / Nchannels
    header_.bytesPerSample = header_.blockAlign / header_.channels;

    // skip the rest of the fmt header ...
    in_.ignore(fmt_chunk_size - 16);
}

// Moves the current read position of in_ to point to the size of a
// data chunk. Assumes the file position is directly before the
// beginning of a new chunk, i.e. the next 4 bytes are the ckID.
void WavDecoder::seek_data() {
    in_.peek(); // make sure eof is set properly if we are at the end of the file
    while (in_.good() && read_string<4>(in_) != "data") skip_chunk();
    if (!in_) {
        throw decoder_error("Couldn't find data chunk!");
    }
}

// Read the next sample from the current data chunk. Seek the next
// chunk if the current chunk is finished.
const WavDecoder::char_buffer& WavDecoder::read_samples(uint32_t nsamples) {
    if (!remaining_chunk_size_) {
        seek_data();
        if (in_.eof()) { // file finished
            buf_.resize(0);
            return buf_;
        }

        // read chunk size
        remaining_chunk_size_ = read_integral<uint32_t>(in_);
        if (!remaining_chunk_size_) {
            throw decoder_error("Empty data chunk!");
        }
    }

    // make sure there is enough space in the buffer to accomodate nsamples
    buf_.resize(static_cast<size_t>(nsamples) * header_.channels, 0);

    // in case not enough samples are available to fully fill buf_ with nsamples_
    if (buf_.size() * header_.bytesPerSample > remaining_chunk_size_) {
        buf_.resize(remaining_chunk_size_ / header_.bytesPerSample, 0);
    }

    if (header_.bitsPerSample == 16) {
        // directly stored as signed short ints, no conversion necessary (except endiadness)
        in_.read(reinterpret_cast<char*>(&buf_[0]), sizeof(int16_t) * buf_.size());
        if (!is_little_endian()) {
            for (int16_t& s : buf_) {
                s = ((s & 0xFF) << 8) | ((s & 0xFF00) >> 8);
            }
        }
    } else if (header_.bitsPerSample == 8) {
        // stored as unsigned chars --> convert to signed short ints
        auto input_it = std::istream_iterator<unsigned char>(in_);
        std::generate_n(buf_.begin(), buf_.size(), [&input_it]() -> int16_t {
            int rv = 257 * (*input_it++) - std::numeric_limits<int16_t>::min(); // [0,255] to [-32768,32767]
            assert(257 * std::numeric_limits<unsigned char>::max() - std::numeric_limits<int16_t>::min() ==
                   std::numeric_limits<int16_t>::max());
            return static_cast<int16_t>(rv);
        });
    } else {
        throw decoder_error("Resolution not supported.");
    }

    remaining_chunk_size_ -= in_.gcount();

    return buf_;
}

} // namespace vscharf
