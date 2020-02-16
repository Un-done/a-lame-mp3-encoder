#include "directory.h"

#include <fstream>

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h> // for
#endif

namespace vscharf {

#ifdef WINDOWS
const char* posix_error::what() const noexcept
{
  LPVOID lpMsgBuf = nullptr;
  FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		 FORMAT_MESSAGE_FROM_SYSTEM,
		 nullptr,
		 _err,
		 0,
		 (LPSTR) &lpMsgBuf,
		 0,
		 nullptr);
  if(!lpMsgBuf) {
    return "";
  } else {
    static thread_local std::string e((LPSTR)lpMsgBuf);
    return e.c_str();
  }
}
#else
const char* posix_error::what() const noexcept
{
  return strerror(_err);
}
#endif // WINDOWS

namespace {
#ifdef WINDOWS
// helper struct that implements RAII for the calls
// FindFirstFileA/FindClose
class scoped_dir {
public:
  scoped_dir(const std::string& path) : path_(path) {
  }
  ~scoped_dir() {
    if(dir_) FindClose(dir_); // ignore error in destructor, can't do anything about it
  }
  std::string next_entry() {
    if(!dir_) {
      // first call
      dir_ = FindFirstFileA(path_.c_str(), &file_);
      if(dir_ == INVALID_HANDLE_VALUE) {
	if(GetLastError() == ERROR_NO_MORE_FILES) {
	  dir_ = nullptr;
	  return {}; // empty string
	}
	throw posix_error(GetLastError());
      }
    } else {
      if(!FindNextFileA(dir_, &file_)) {
	if(GetLastError() == ERROR_NO_MORE_FILES) return {}; // empty string
	throw posix_error(GetLastError());
      }
    }
    return file_.cFileName;
  }
private:
  HANDLE dir_ = nullptr;
  WIN32_FIND_DATAA file_;
  std::string path_;

};

#else
// helper struct that implements RAII for the paired opendir/closedir
// calls
class scoped_dir {
public:
  scoped_dir(const std::string& path) : dir_(opendir(path.c_str())) {
    if(!dir_) throw posix_error(errno);
  }
  ~scoped_dir() {
    closedir(dir_); // ignore error as destructor must never fail
  }
  DIR* dir() const { return dir_; }
private:
  DIR* dir_;
}; // class scoped_dir
#endif
} // anonymous namespace

// take argument by value as it is modified later
std::vector<std::string> directory_entries(std::string path)
{
  std::vector<std::string> entries;
#ifdef WINDOWS
  scoped_dir dir(path + "\\*");
  path += '\\';
  std::string filename;
  while(!(filename = dir.next_entry()).empty()) {
    entries.push_back(path + std::move(filename));
  }
#else
  scoped_dir dir(path);
  errno = 0;
  dirent* current = nullptr;
  path += '/';
  while((current = readdir(dir.dir())) != nullptr) {
    entries.push_back(path + current->d_name);
  }
  if(errno) throw posix_error(errno);
#endif
  return entries; // rely on copy ellision / move
} // directory_entries

} // namespace vscharf
