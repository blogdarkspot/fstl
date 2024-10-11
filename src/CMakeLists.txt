cmake_minimum_required(VERSION 3.20)

add_library(containers  INTERFACE)
target_include_directories(containers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_features(containers INTERFACE cxx_std_20)
#target_compile_options(containers INTERFACE -fprofile-arcs -ftest-coverage)
add_library(cstl::container ALIAS containers)
