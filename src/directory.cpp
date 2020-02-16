#include "directory.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

#ifdef WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <errno.h>
    #include <string.h>
#endif

#include <iostream>

namespace vscharf {

#ifdef WINDOWS
const char* posix_error::what() const noexcept {
    LPVOID lpMsgBuf = nullptr;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, _err, 0, (LPSTR) &lpMsgBuf, 0, nullptr);
    if (!lpMsgBuf) {
        return "";
    } else {
        static thread_local std::string e((LPSTR) lpMsgBuf); // :gut:
        return e.c_str();
    }
}
#else
const char* posix_error::what() const noexcept {
    return strerror(_err);
}
#endif // WINDOWS

std::vector<std::filesystem::path> directory_entries(std::filesystem::path path) {
    std::vector<std::filesystem::path> entries;

    // TODO handle access error permission denied etc
    // NOTE: For a real app this could become a generator there is not
    // need to materialze n directroy entries.
    std::filesystem::recursive_directory_iterator dir_iter(path);

    for (auto const& p : dir_iter) {
        // TODO - Filter for filetypes here
        entries.push_back(p);
    }

    return entries;
} // directory_entries

// lets keep this version for testsing.
std::vector<std::string> directory_entries(std::string path) {
    auto res = directory_entries(std::filesystem::path(path));
    std::vector<std::string> rv;

    std::transform(res.begin(), res.end(), std::back_inserter(rv), [](std::filesystem::path const& item) {
        return item.native();
    });

    return rv;
}

} // namespace vscharf
