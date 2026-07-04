cmake_minimum_required(VERSION 3.20)

project(Albert LANGUAGES CXX)

add_executable(albert
    src/main.cpp
    src/operators.cpp
    src/memory.cpp
    src/attributes.cpp
    src/op/aif.cpp
    src/op/rex.cpp
    src/out/binary.cpp
    src/out/asm.cpp
)

target_compile_features(albert PRIVATE cxx_std_20)

# Optional but recommended: keep target output in a predictable place.
set_target_properties(albert PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Future growth pattern:
# Add new modules as more .cpp files and append them to this same target.
# Example:
# target_sources(albert PRIVATE src/new_module.cpp)