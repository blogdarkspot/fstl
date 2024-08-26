cmake_minimum_required(VERSION 3.20)

add_subdirectory("${PROJECT_SOURCE_DIR}/submodules/googletest" "submodules/googletest")
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
add_executable(UnitTests)
target_sources(UnitTests PRIVATE LinkedListTests.cpp 
				 HashTableTests.cpp
				 AlgorithmsTests.cpp
				 BinaryTreeTests.cpp 
				 GraphTests.cpp "SmartPtrTest.cpp")
target_include_directories(UnitTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(UnitTests PRIVATE GTest::gtest_main)
target_compile_features(UnitTests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(UnitTests)
