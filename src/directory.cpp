#include "directory.h"

#include <algorithm>
#include <filesystem>


namespace vscharf {

auto directory_entries(std::filesystem::path const &path) -> std::vector<std::filesystem::path> {
    std::vector<std::filesystem::path> entries;

    // TODO handle access error permission denied etc
    // NOTE: For a real app this could become a generator there is not
    // need to materialze n directroy entries.
    std::filesystem::recursive_directory_iterator dir_iter(path);

    for (auto const& p : dir_iter) {
        if (p.is_regular_file()) {
            auto filePath = p.path();
            if (filePath.has_extension()) {
                auto extension = filePath.extension().string();
                // Transform to lowercase to get all variations of wav
                // Will work until non-Ascii characters are used and modulo to "wav"
                std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
                if (extension == ".wav") {
                    entries.push_back(std::move(filePath));
                }
            }
        }
    }

    return entries;
} // directory_entries

} // namespace vscharf
