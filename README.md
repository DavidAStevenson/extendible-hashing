# extendible-hashing
An old project from my university days refreshed to modern C++, with automated testing added

- Catch2 is used testing.

Basic compiliation and running notes:
```
make [catch2test] [gtest]
```
Running catch2 tests:
```
./catch2test
```
Running googletest tests:
```
./gtest
```
Cleaning up
```
make clean
```

# Status
- bit operations library compiled and has tests
  - some tests with catch2
  - more tests with googletest (better coverage)
- indexholder discovered to not even be calling any bit_op_lib functions, however!
  - so next step is to add tests on indexholder
  - there is an old test file that may give some useful tips on testing / using it
  - perhaps the intent had been to extra bit operations out of indexholder, into bit_op_lib?
  - if so, adding tests around indexholder first, then changing indexholder implementation to use bit_op_lib could be the way to go.

# TODO

- Complete port of the bit operations library, with tests
- Add a trimmed down Vagrantfile with just the necessary items to build and run

# NOTES
- some references I found useful for setting up googletest
  - http://xvideos.hatenablog.com/entry/2014/11/24/022149
  - http://xvideos.hatenablog.com/entry/google_test_in_project
  - https://qiita.com/tjun/items/8047bf50930e6de36bdc
- the best article I found about creating generic Makefiles
  - https://latedev.wordpress.com/2014/11/08/generic-makefiles-with-gcc-and-gnu-make/
  - https://latedev.wordpress.com/2014/12/02/generic-makefiles-with-gcc-and-gnu-make-part-2/

