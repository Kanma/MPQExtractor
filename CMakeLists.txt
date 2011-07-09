project(MPQEXTRACTOR)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MPQEXTRACTOR_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MPQEXTRACTOR_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MPQEXTRACTOR_BINARY_DIR}/bin")

add_subdirectory(StormLib)

include_directories("${MPQEXTRACTOR_SOURCE_DIR}/StormLib/src/"
                    "${MPQEXTRACTOR_SOURCE_DIR}/include/"
)

add_executable(MPQExtractor main.cpp)
target_link_libraries(MPQExtractor StormLib)

# Set the RPATH
if (APPLE)
    set_target_properties(MPQExtractor PROPERTIES LINK_FLAGS "-Wl,-rpath,@loader_path/.")
elseif (UNIX)
    set_target_properties(MPQExtractor PROPERTIES INSTALL_RPATH ".")
endif()
