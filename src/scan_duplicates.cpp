#include "bayan.hpp"

namespace otus {

namespace utils {

static std::string
sha1sum(const std::string &data) {
    boost::compute::detail::sha1 sha1{data};
    return {sha1};
}

static std::vector<fs::path>
get_filepaths(const fs::path &dir, std::vector<fs::path> excl, bool recursive) {
    std::vector<fs::path> paths;
    auto push = [&dir, &excl, &paths](const fs::directory_entry &entry) {
        if (fs::is_directory(entry) ||
            std::find(excl.begin(), excl.end(), entry) != excl.end()) {
            return;
        }

        paths.push_back(dir / fs::relative(entry.path(), dir));
    };

    if (recursive) {
        for (const auto &p : fs::recursive_directory_iterator(
                 dir, fs::directory_options::pop_on_error)) {
            push(p);
        }
    } else {
        for (const auto &p : fs::directory_iterator(dir)) {
            push(p);
        }
    }

    return paths;
}

static bool
is_equal(const fs::path &a, const fs::path &b, size_t file_size,
         size_t chunk_size) {
    std::ifstream a_stream(a);
    BOOST_ASSERT_MSG(a_stream.is_open(),
                     std::format("{} open failed!", a.c_str()).c_str());
    std::ifstream b_stream(b);
    BOOST_ASSERT_MSG(b_stream.is_open(),
                     std::format("{} open failed!", b.c_str()).c_str());

    std::string a_buf, b_buf;
    a_buf.reserve(chunk_size);
    b_buf.reserve(chunk_size);

    bool equal = true;
    size_t counter = file_size;
    while (counter) {
        size_t residual = counter < chunk_size ? counter : chunk_size;
        counter -= residual;

        a_stream.read(&a_buf[0], residual);
        b_stream.read(&b_buf[0], residual);
        if (sha1sum(a_buf) != sha1sum(b_buf)) {
            equal = false;
            break;
        }
    }

    a_stream.close();
    b_stream.close();
    return equal;
}

bool
is_vmap_cointain(const std::map<fs::path, std::vector<fs::path>> &map,
                 const fs::path &entry) {
    bool result = false;
    for (auto &&[key, vec] : map) {
        result |= std::find(vec.begin(), vec.end(), entry) != vec.end();
        if (result) {
            return result;
        }
    }

    return result;
}

}   // namespace utils

ScanDup::ScanDup(const std::vector<fs::path> &directories,
                 const std::vector<fs::path> &excludes,
                 std::vector<std::string> &masks, bool recursive,
                 size_t min_size, size_t block_size)
    : dir_list_(directories), exclude_list_(excludes), masks_list_(masks),
      recursive_(recursive), min_size_(min_size), block_size_(block_size) {

    BOOST_ASSERT_MSG(!directories.empty(), "No paths provided!");
    for (auto &&dir : dir_list_) {
        BOOST_ASSERT_MSG(
            fs::is_directory(dir),
            std::format("{} not a directory!", dir.c_str()).c_str());
    }
}

void
ScanDup::result() {
    using namespace utils;

    // Get files list
    std::vector<fs::path> all_files;
    for (auto &&dir : dir_list_) {
        auto files = get_filepaths(dir, exclude_list_, recursive_);
        all_files.insert(all_files.end(), files.begin(), files.end());
    }

    // Delete not masked files, if mask is presented
    if (masks_list_.size()) {
        for (auto &&mask : masks_list_) {
            const boost::regex filter(mask, boost::regex::icase);
            auto mask_eraser = [&filter](const fs::path &x) {
                boost::smatch what;
                return !boost::regex_match(x.filename().string(), what, filter);
            };

            all_files.erase(std::remove_if(all_files.begin(), all_files.end(),
                                           mask_eraser));
        }
    }

    // To filter files by size
    std::map<std::string, size_t> size_map;
    for (auto &&file : all_files) {
        size_map[file.filename().string()] = fs::file_size(file);
    }

    // Delete small files
    auto msize = min_size_;
    auto min_eraser = [&size_map, &msize](const fs::path &x) {
        const auto path = x.filename().string();
        bool less = size_map[path] < msize;

        if (less)
            size_map.erase(path);
        return less;
    };
    all_files.erase(
        std::remove_if(all_files.begin(), all_files.end(), min_eraser),
        all_files.end());

    // Fill map_
    BOOST_ASSERT_MSG(all_files.size() > 1, "Less than 2 files found!");
    for (auto &&file : all_files) {
        // Why just not to compute sha1 of the whole files and find duplicates
        // linearly?
        for (auto &&other : all_files) {
            auto size = size_map[other.filename().string()];
            if (other.compare(file) == 0 ||
                size != size_map[file.filename().string()] ||
                map_.contains(file)) {
                continue;
            }

            if (is_equal(file, other, size, block_size_) &&
                !is_vmap_cointain(map_, file)) {
                map_[file].push_back(other);
            }
        }
    }

    // Print result
    for (auto &&[key, value] : map_) {
        std::cout << key << '\n';
        for (auto &&nested : value) {
            std::cout << nested << '\n';
        }
        std::cout << std::endl;
    }
}
}   // namespace otus
