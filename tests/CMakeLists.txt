cmake_minimum_required(VERSION 3.5...3.30)

include(FetchContent)
set(BUILD_GMOCK ON CACHE BOOL "Build GMock" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Disable install" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "Force shared CRT" FORCE)
FetchContent_Declare(
	GoogleTest
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(GoogleTest)

add_executable(tests)

target_sources(tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_containers/test_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_containers/test_sorted_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_containers/test_vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_containers/test_stack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tables/test_hash_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tables/test_rbtree_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tables/test_unsorted_array_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tables/test_sorted_array_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_polynomial/test_monomial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_polynomial/test_polynomial.cpp
)

target_link_libraries(tests
	PRIVATE
	polynomial_lib
    containers
    tables
	GTest::gtest_main
	GTest::gmock
)

target_include_directories(tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

include(GoogleTest)
gtest_discover_tests(tests)