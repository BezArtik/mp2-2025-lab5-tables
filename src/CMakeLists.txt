add_library(polynomial_lib STATIC
    polynomial/polynomial.cpp
    polynomial/monomial.cpp
)

target_include_directories(polynomial_lib 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/polynomial
)

target_link_libraries(polynomial_lib 
    PUBLIC
        containers  
)

target_compile_features(polynomial_lib PUBLIC cxx_std_20)

add_library(project::polynomial ALIAS polynomial_lib)


add_library(project_compiled INTERFACE)
target_link_libraries(project_compiled INTERFACE
    polynomial_lib
)