cmake_minimum_required(VERSION 3.4.3)
project(arctic)

find_package(LLVM REQUIRED CONFIG)
find_package(LLD REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using LLDConfig.cmake in: ${LLD_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${LLD_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core support transformutils)

message(STATUS "Using LLVM libraries: ${llvm_libs}")
message(STATUS "Using LLD libraries: ${LLD_EXPORTED_TARGETS}")

file(GLOB_RECURSE ARC_SRC
    src/*.cpp
)

add_executable(arc ${ARC_SRC})
target_link_libraries(arc PRIVATE ${llvm_libs})
target_link_libraries(arc PRIVATE ${LLD_EXPORTED_TARGETS})
