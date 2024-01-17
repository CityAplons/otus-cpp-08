#pragma once

#include <cstddef>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <boost/assert.hpp>
#include <boost/compute/detail/sha1.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace otus {

namespace fs = boost::filesystem;

class ScanDup {
  public:
    explicit ScanDup(const std::vector<fs::path> &directories,
                     const std::vector<fs::path> &excludes,
                     std::vector<std::string> &masks, bool recursive,
                     size_t min_size, size_t block_size);
    ~ScanDup() {}

    void result();

  private:
    std::map<fs::path, std::vector<fs::path>> map_;
    std::vector<fs::path> dir_list_;
    std::vector<fs::path> exclude_list_;
    std::vector<std::string> masks_list_;

    bool recursive_;
    size_t min_size_;
    size_t block_size_;
};

}   // namespace otus
