cmake_minimum_required(VERSION 3.11)
project(sousflow CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_Declare(
  utest
  GIT_REPOSITORY https://github.com/sheredom/utest.h.git
  GIT_TAG master
)
FetchContent_MakeAvailable(yaml-cpp utest)

add_library(sousflow STATIC 
    src/sousflow.cpp
    include/sousflow.h
)
target_include_directories(sousflow PUBLIC 
    include
    ${yaml-cpp_SOURCE_DIRECTORY/include}
)
target_link_libraries(sousflow PUBLIC 
    yaml-cpp::yaml-cpp
)


enable_testing()
add_test(NAME sousflow_test COMMAND sousflow_test)

add_executable(sousflow_test 
    test/test_parser.cpp
)
target_link_libraries(sousflow_test PUBLIC sousflow)
target_include_directories(sousflow_test PUBLIC 
    include
    ${utest_SOURCE_DIR}
)