## uVec - a type-safe, generic C vector.

### ⚠️ Important note ⚠️

uVec is now part of the [uLib](https://github.com/IvanoBilenchi/ulib) library. All further development has moved there.

### Author

[Ivano Bilenchi](https://ivanobilenchi.com)

### Description

Implementation of a type-safe, generic vector data structure written in C.
Macro-heavy, not for the faint of heart.
Inspired by [klib](https://github.com/attractivechaos/klib).

### Features

- Vector primitives (`uvec_get`, `uvec_set`, `uvec_push`, `uvec_pop`, `uvec_append`, ...)
- Iteration macros (`uvec_iterate`, `uvec_foreach`, ...)
- Support for element equality and related features (`uvec_contains`, `uvec_index_of`, ...)
- Support for element comparison and related features (`uvec_index_of_max`, `uvec_index_of_min`, `uvec_sort`, ...)
- Higher order macros (`uvec_first_index_where`, `uvec_remove_where`, ...)

### Usage

If you are using [CMake](https://cmake.org) as your build system, you can add `uVec` as
a subproject, then link against the `uvec` target. Otherwise, in general you just need
the [uvec.h](include/uvec.h) header.

### Documentation

Documentation for the project is provided in form of docstrings in the *Public API* section
of [uvec.h](include/uvec.h). You can also generate HTML docs via CMake, though you will
also need [Doxygen](http://www.doxygen.nl). For usage examples, see [test.c](test/test.c).

### CMake targets

- `uvec`: interface library target, which you can link against.
- `uvec-docs`: generates documentation via Doxygen.
- `uvec-test`: generates the test suite.

### License

`uVec` is available under the MIT license. See the [LICENSE](./LICENSE) file for more info.
