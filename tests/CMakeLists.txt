enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 11)

add_executable(unittest ffm_test.cpp)
target_include_directories(unittest PRIVATE ..)
target_link_libraries(unittest ffm)
target_link_libraries(unittest GTest::GTest GTest::Main)

add_custom_target(test unittest)
