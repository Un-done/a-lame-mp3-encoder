#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <vector>

#include "directory.h"
#include "mp3encoder.h"
#include "wavdecoder.h"

using namespace vscharf;

// Function for encoding a file
void do_work(std::filesystem::path const &infile_path) {
        // do the encoding
        std::filesystem::path outfile_path(infile_path);
        outfile_path.replace_extension(".mp3");

        std::ifstream infile(infile_path);
        std::ofstream outfile(outfile_path);
        // todo open?

        WavDecoder wav(infile);
        Mp3Encoder mp3(2);
        mp3.encode(wav, outfile);
} // do_work

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << argv[0] << ": missing directory operand" << std::endl;
        return 1;
    } else if (argc > 2) {
        std::cerr << argv[0] << ": too many directory operands" << std::endl;
        return 2;
    }

    std::filesystem::path dir(argv[1]);
    std::vector<std::filesystem::path> wav_files = directory_entries(dir);


    auto wav_files = directory_entries(dir);

    std::for_each(std::execution::par_unseq, wav_files.begin(), wav_files.end(), [](auto const& path) {
            do_work(path);
        });

    std::cout << "Successfully converted " << wav_files.size() << " WAV files to mp3." << std::endl;

    return 0;
}
