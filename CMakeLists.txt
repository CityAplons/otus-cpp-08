cmake_minimum_required(VERSION 3.10)

set(PROJECT_REVISION 1 CACHE INTERNAL "Project revision (CI)")
project(bayan
    VERSION 0.0.${PROJECT_REVISION}
    DESCRIPTION "Find same file in a directory"
    HOMEPAGE_URL "https://github.com/CityAplons/otus-cpp-08"
    LANGUAGES CXX
)

set_property(GLOBAL PROPERTY G_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY G_CXX_STANDARD_REQUIRED ON)

# Boost
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS filesystem program_options locale regex) 

# Include utility functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(compiler)
include(test)

# Transparent versioning
include_directories(cmake/version)
configure_file(cmake/version/version.h.in version.h)
configure_file(cmake/version/VERSION.in VERSION)
add_library(version cmake/version/project.cpp)
target_include_directories(version
    PRIVATE "${CMAKE_BINARY_DIR}"
)

# Describe build targets
add_executable(bayan src/main.cpp src/scan_duplicates.cpp)
target_link_libraries(bayan PRIVATE
    version
    Boost::regex
    Boost::locale
    Boost::filesystem
    Boost::program_options
)

set_compile_options(bayan)
set_compile_options(version)

install(TARGETS bayan
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Describe unit tests
enable_testing()
add_unit_test(test/test_version.cpp version)

# Packaging
option(MAKE_PACKAGE "Add 'package' and 'package_source' build targets for packages" ON)
if (MAKE_PACKAGE)
	add_subdirectory(cmake/packaging)
endif()