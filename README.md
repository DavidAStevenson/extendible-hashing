# extendible-hashing
An old project from my university days refreshed to modern C++, with automated testing added

- Catch2 is used for testing. (Not included in this repo.)
- GoogleTest is also used for testing.

The compilation prerequisites are included in [this Vagrantfile repo](https://github.com/DavidAStevenson/dev-host "dev-host"), so get that first if necessary.

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
- various lib/ sources now compiled and have tests (combination of googletest and catch2)
- indexholder discovered to not even be calling any bit_op_lib functions, however! I probably had intended to refactor common operations into it, but never completed the job.
  - later look to change indexholder implementation to use bit_op_lib?
  - indexholder interface is unclear
    - initial tests were failing, but after understanding the expected usage, resolved it
	- rejuvenating this old code is an exercise, but typically something should be done about the interface
	  - documentation at minimum, alternatively a better interface / implementation would be best
- look to "modernize" the source
  - e.g. 
  	- <string> rather than char strings? 
	- enums
- other refactoring
  - some of the API is not so caller / test friendly

# TODO
- Complete tests around main extendiblehashfile
- Tidy up TODO items throughout source

# NOTES
- some references I found useful for setting up googletest
  - http://xvideos.hatenablog.com/entry/2014/11/24/022149
  - http://xvideos.hatenablog.com/entry/google_test_in_project
  - https://qiita.com/tjun/items/8047bf50930e6de36bdc
- the best article I found about creating generic Makefiles
  - https://latedev.wordpress.com/2014/11/08/generic-makefiles-with-gcc-and-gnu-make/
  - https://latedev.wordpress.com/2014/12/02/generic-makefiles-with-gcc-and-gnu-make-part-2/

# LESSONS LEARNED
- surprisingly, I was using plain "delete" against arrays in the original code
  - when deleting arrays created on the heap with new, delete[] must be used, not delete
  - this tidbit got a brief mention in my old C++ text, but perhaps I never knew this? A NetBSD compiler quirk?
- don't waste time going down the rabbit hole of memory issues
  - just install valgrind, and learn the basics
  - upon using valgrind, I was quickly able to identify an issue that resulted in program crashes
  	- probably, back in 2000/2001, I had got stuck with this issue and that was where I left the code
  - gdb also essential
