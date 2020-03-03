## Vector(T) - a type-safe, generic C vector.

### Author

[Ivano Bilenchi](https://ivanobilenchi.com)

### Description

Implementation of a type-safe, generic vector data structure written in C.
Macro-heavy, not for the faint of heart.
Inspired by [klib](https://github.com/attractivechaos/klib).

### Features

- Vector primitives (`vector_get`, `vector_set`, `vector_push`, `vector_pop`, `vector_append`, ...)
- Iteration macros (`vector_iterate`, `vector_foreach`, ...)
- Support for element equality and related features (`vector_contains`, `vector_index_of`, ...)
- Support for element comparison and related features (`vector_index_of_max`, `vector_index_of_min`, `vector_sort`, ...)
- Higher order macros (`vector_first_index_where`, `vector_remove_where`, ...)

### Usage

If you are using [CMake](https://cmake.org) as your build system, you can add `Vector(T)` as
a subproject, then link against the `vector` target. Otherwise, in general you just need
the [vector.h](include/vector.h) header.

### Documentation

Documentation for the project is provided in form of docstrings in the *Public API* section
of [vector.h](include/vector.h). You can also generate HTML docs via CMake, though you will
also need [Doxygen](http://www.doxygen.nl). For usage examples, see [test.c](test/test.c).

### CMake targets

- `vector`: interface library target, which you can link against.
- `vector-docs`: generates documentation via Doxygen.
- `vector-test`: generates the test suite.

### License

`Vector(T)` is available under the MIT license. See the [LICENSE](./LICENSE) file for more info.
