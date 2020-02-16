#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "directory.h"
#include "mp3encoder.h"
#include "wavdecoder.h"

#include <mutex>
#include <thread>

using namespace vscharf;

// Function for encoding a file taken from the (condition-protected)
// list of files-names available_files. After no file is left
// decreases the (again condition-protected) number of available
// threads.
namespace EncodeFiles {
void do_work(std::mutex& mut, std::vector<std::filesystem::path> available_files) {
    while (1) {
        std::filesystem::path infile_path;
        {
            std::scoped_lock gurad(mut);
            if (available_files.empty()) {
                return;
            }
            infile_path = available_files.back();
            available_files.pop_back();
        }

        // do the encoding
        std::filesystem::path outfile_path(infile_path);
        outfile_path.replace_extension(".mp3");

        std::ifstream infile(infile_path);
        std::ofstream outfile(outfile_path);
        // todo open?

        WavDecoder wav(infile);
        Mp3Encoder mp3(2);
        mp3.encode(wav, outfile);
    }
    // not reachable
    return;
} // do_work
} // namespace EncodeFiles

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


    // mutex protected list of remaining files
    const std::size_t n_wav_files = wav_files.size();
    std::vector<std::filesystem::path> available_files(wav_files);
    std::mutex mut;

    // do the work on (joinable) threads
    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
        threads.emplace_back([&mut, &available_files]() {
            EncodeFiles::do_work(mut, available_files);
        });
    }

    // wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Successfully converted " << n_wav_files << " WAV files to mp3." << std::endl;

    return 0;
}
