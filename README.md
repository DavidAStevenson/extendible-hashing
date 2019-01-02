# extendible-hashing
An old project from my university days refreshed to modern C++, with automated testing added

- Catch2 is used testing.

Basic compiliation and running notes:
```
g++ -Wall -std=c++14 -c bit_op_lib.cc && \
g++ -Wall -std=c++14 bit_op_lib.o bit_op_lib_test.cc && \
./a.out

```

TODO
- Complete port of the bit operations library, with tests
- Do some testing with googletest framework as well, for comparision
- Add a trimmed down Vagrantfile with just the necessary items to build and run
- More c++11,14 study
- More reading of Google's C++ style guide
- Figure out the problem with my namespace declarations
