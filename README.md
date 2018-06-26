## Vector(T) - a type-safe, generic C vector.

### Author

Ivano Bilenchi ([@IvanoBilenchi](http://www.twitter.com/IvanoBilenchi))


### Description

Implementation of a type-safe, generic vector data structure written in C.
Macro-heavy, not for the faint of heart. Inspired by [klib](https://github.com/attractivechaos/klib).


### Documentation

See comments in the *Public API* section of [vector.h](include/vector.h). For usage examples, see [test.c](test/test.c).
To run the tests: `cd test`, then `make run`.


### Features

- Vector primitives (`vector_get`, `vector_set`, `vector_push`, `vector_pop`, `vector_append`, ...)
- Iteration macros (`vector_iterate`, `vector_foreach`, ...)
- Support for element equality and related features (`vector_contains`, `vector_index_of`, ...)
- Higher order macros (`vector_first_index_where`, `vector_remove_where`, ...)


### Todo

- [x] Documentation
- [x] Tests
- [ ] Better documentation (html? usage examples?)
- [ ] Better tests
- [ ] More goodies (sorting, reversing, ...)


### License

`Vector(T)` is available under the MIT license. See the [LICENSE](LICENSE) file for more info.
