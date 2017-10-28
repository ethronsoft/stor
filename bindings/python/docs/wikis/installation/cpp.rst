.. _cpp-inst:



STOR Installation
===================

STOR uses `CMake <https://cmake.org/Wiki/CMake>`_ for builds 

Get a copy of the repository::

    git clone https://github.com/ethronsoft/stor
    
The files of interest to build the C++ version of STOR are in the
**stor/core** folder

To build the library, make an out of source directory::
   
    mkdir cpp_stor_build

Then setup the project Cmake cache and relative files that will be used during the build::

   chdir cpp_stor_build
   cmake [OPT] ../stor/core
   
.. note::
     OPT refers to the various CMake options you can use, such as:
      - `-G "Unix Makefiles` 
      - `-G "MinGW Makefiles"`
     
     This is the place where to use option `-DSTOR_CRYPTO=ON`
     if you plan on using the encrypting facilities of the database.

Then build the static library::

   cmake --build . --target stor

And run the tests::

   cmake --build . --target stor_test
   ./stor_test

.. note::
    If in doubt about what dependencies to link against your executable,
    check out `target_link_libraries(${PROJECT_NAME}_test ..)` in CmakeLists.txt