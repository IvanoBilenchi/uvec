# Test target

if(MSVC)
    set(VEC_WARNING_OPTIONS /W4)
else()
    set(VEC_WARNING_OPTIONS -Wall -Wextra)
endif()

add_executable(vector-test "${VEC_TEST_DIR}/test.c")
target_compile_options(vector-test PRIVATE ${VEC_WARNING_OPTIONS})
target_link_libraries(vector-test PRIVATE vector)
