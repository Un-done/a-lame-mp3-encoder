// -*- C++ -*-
#ifndef ALAMEMP3ENCODER_DIRECTORY_H
#define ALAMEMP3ENCODER_DIRECTORY_H

#include <filesystem>
#include <vector>

namespace vscharf {

// ======== functions ========
auto directory_entries(std::filesystem::path const &path) -> std::vector<std::filesystem::path>;
} // namespace vscharf

#endif // ALAMEMP3ENCODER_DIRECTORY_H
