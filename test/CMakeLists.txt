# Test target

if(MSVC)
    set(VEC_WARNING_OPTIONS /W4)
else()
    set(VEC_WARNING_OPTIONS -Wall -Wextra)
endif()

add_executable(uvec-test "test.c")
target_compile_options(uvec-test PRIVATE ${VEC_WARNING_OPTIONS})
target_link_libraries(uvec-test PRIVATE uvec)
