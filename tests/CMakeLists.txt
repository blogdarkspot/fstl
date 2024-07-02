cmake_minimum_required(VERSION 3.20)

add_subdirectory("${PROJECT_SOURCE_DIR}/submodules/googletest" "submodules/googletest")
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
add_executable(HashTableTests)
target_sources(HashTableTests PRIVATE HashTableTests.cpp "HeapTests.cpp" "BinaryTreeTests.cpp")
target_include_directories(HashTableTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(HashTableTests PRIVATE GTest::gtest_main)
target_compile_features(HashTableTests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(HashTableTests)
