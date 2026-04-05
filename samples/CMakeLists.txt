function(add_sample sample_name source_dir source_file)
    add_executable(${sample_name} ${source_dir}/${source_file})
    
    target_link_libraries(${sample_name}
        PRIVATE
            containers
            tables
            project_compiled  
    )
    
    target_compile_features(${sample_name} PRIVATE cxx_std_20)
    
    set_target_properties(${sample_name} PROPERTIES
        FOLDER "samples/${source_dir}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/samples/${source_dir}"
    )
endfunction()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/polynomial/sample_polynomial.cpp)
    add_sample(polynomial_sample polynomial sample_polynomial.cpp)
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/sample_benchmark.cpp)
    add_sample(benchmark_sample benchmark sample_benchmark.cpp)
endif()