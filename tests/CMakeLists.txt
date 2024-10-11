cmake_minimum_required(VERSION 3.20)
Include(FetchContent)
FetchContent_Declare(
    GoogleTest
    GIT_REPOSITORY https://github.com/google/googletest.git 
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(GoogleTest)

set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
add_executable(UnitTests)
target_sources(UnitTests PRIVATE  
                 "span_ranges_tests.cpp"
                 "interval_tree_tests.cpp")
				 #"order_statistics_tests.cpp")
#target_compile_options(UnitTests PUBLIC --coverage -fprofile-arcs -ftest-coverage)
target_compile_features(UnitTests PRIVATE cxx_std_20)
target_compile_options(UnitTests PRIVATE -fprofile-arcs -ftest-coverage)
target_link_libraries(UnitTests PRIVATE GTest::gtest_main cstl::container gcov)
include(GoogleTest)
gtest_discover_tests(UnitTests)
