#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>

#include "bayan.hpp"
#include "project.h"

int
main(int argc, char const *argv[]) {
    struct ProjectInfo info = {};
    bool isRecursive = false;
    size_t minimumSize = 1, chunkSize = 5;

    std::cout << info.nameString << "\t" << info.versionString << '\n';

    namespace po = boost::program_options;
    po::options_description desc("Find same file in a directory");
    desc.add_options()("help,h", "Print this message")(
        "directories,d",
        po::value<std::vector<boost::filesystem::path>>()
            ->multitoken()
            ->required(),
        "List of directories to scan")(
        "exclude,e",
        po::value<std::vector<boost::filesystem::path>>()->multitoken(),
        "List of directories to exclude")(
        "mask,m",
        po::value<std::vector<boost::filesystem::path>>()->multitoken(),
        "List of file masks to process")("recursive,r",
                                         po::bool_switch(&isRecursive),
                                         "Use recursive scan. Default: false")(
        "min_file_size,s", po::value(&minimumSize),
        "Minimum file size to process, bytes. Default: 1")(
        "chunk_size,c", po::value<uint64_t>(&chunkSize),
        "Maximum chunk size for reading a file. Default: 5");

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const std::exception &e) {
        if (vm.count("help")) {
            std::cout << desc << "\n";
        }
        std::cerr << e.what() << '\n';
        return 2;
    }

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    using namespace otus;
    auto exclude =
        vm.count("exclude")
            ? vm.at("exclude").as<std::vector<boost::filesystem::path>>()
            : std::vector<boost::filesystem::path>();
    auto mask = vm.count("mask") ? vm.at("mask").as<std::vector<std::string>>()
                                 : std::vector<std::string>();
    ScanDup impl(
        vm.at("directories").as<std::vector<boost::filesystem::path>>(),
        exclude, mask, isRecursive, minimumSize, chunkSize);

    impl.result();
    return 0;
}
