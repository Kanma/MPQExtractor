cmake_minimum_required(VERSION 3.16)

project(MPQEXTRACTOR)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MPQEXTRACTOR_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MPQEXTRACTOR_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MPQEXTRACTOR_BINARY_DIR}/bin")


if (NOT EXISTS "${MPQEXTRACTOR_SOURCE_DIR}/StormLib/CMakeLists.txt")
    message(FATAL_ERROR
"Missing dependency: StormLib
MPQExtractor requires the StormLib library.
It is provided as a GIT submodule of this repository.
Did you forgot to execute the following commands?
   git submodule init
   git submodule update")
endif()

# Recent StormLib can link libtommath/libtomcrypt either from the system (via
# pkg-config) or from its bundled copies. Default to the bundled ones so that
# a fresh clone still builds with nothing but a compiler and CMake, as it did
# before. Override with -DSTORM_USE_BUNDLED_LIBRARIES=OFF to link system
# libraries instead.
option(STORM_USE_BUNDLED_LIBRARIES
    "Force use of bundled dependencies instead of system libraries." ON
)


add_subdirectory(StormLib)

include_directories("${MPQEXTRACTOR_SOURCE_DIR}/StormLib/src/"
                    "${MPQEXTRACTOR_SOURCE_DIR}/include/"
)

# Replacement for dirent.h and unistd.h
if (WIN32)
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dirent/include")
endif()

add_executable(MPQExtractor main.cpp)
target_link_libraries(MPQExtractor storm)

# Set the RPATH
if (APPLE)
    set_target_properties(MPQExtractor PROPERTIES LINK_FLAGS "-Wl,-rpath,@loader_path/.")
elseif (UNIX)
    set_target_properties(MPQExtractor PROPERTIES INSTALL_RPATH ".")
endif()
