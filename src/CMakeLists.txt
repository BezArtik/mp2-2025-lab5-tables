add_library(polynomial_lib STATIC
    polynomial/polynomial.cpp
    polynomial/monomial.cpp
    hash/monomial_hash.cpp
    hash/polynomial_hash.cpp
    hash/murmurhash.cpp
    generators/generators.cpp
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

add_library(project::polynomial ALIAS polynomial_lib)