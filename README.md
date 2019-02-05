## Vector(T) - a type-safe, generic C vector.

### Author

[Ivano Bilenchi](https://ivanobilenchi.com)


### Description

Implementation of a type-safe, generic vector data structure written in C.
Macro-heavy, not for the faint of heart. Inspired by [klib](https://github.com/attractivechaos/klib).


### Documentation

Documentation for the project is provided in form of docstrings in the *Public API* section of [vector.h](include/vector.h). HTML and LaTeX docs can be generated via [Doxygen](http://www.doxygen.nl). For usage examples, see [test.c](test/test.c).

**Run tests:** `cd test && make run`

**Generate docs:** `doxygen Doxyfile`


### Features

- Vector primitives (`vector_get`, `vector_set`, `vector_push`, `vector_pop`, `vector_append`, ...)
- Iteration macros (`vector_iterate`, `vector_foreach`, ...)
- Support for element equality and related features (`vector_contains`, `vector_index_of`, ...)
- Support for element comparison and related features (`vector_max`, `vector_min`, `vector_sort`, ...)
- Higher order macros (`vector_first_index_where`, `vector_remove_where`, ...)


### License

`Vector(T)` is available under the MIT license. See the [LICENSE](./LICENSE) file for more info.
