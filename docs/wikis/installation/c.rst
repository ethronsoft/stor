.. _c-bindings-inst:

C Bindings Installation
=======================

CSTOR uses `CMake <https://cmake.org/Wiki/CMake>`_ for builds 

Get a copy of the repository::

    git clone https://github.com/ethronsoft/stor
    
The files of interest to build the C version of STOR are in the
**stor/bindings/c** folder

.. note::
    What is happening here is that CSTOR will need to build STOR
    and it will then build a **C dynamic library statically linked 
    against STOR and dependencies** (STOR and dependencies get compiled as position independent code).
    This will allow the user of the C library to only
    dynamically link against CSTOR. 
    CMake will take care of this for you, but now you know, in
    case you want to tweak this part. 

.. note::
     This is C++ wrapper written in C, so a C++ compiler is used by CMake. 
    
To build the library, make an out of source directory::
   
    mkdir c_stor_build

Then set up the project Cmake cache and relative files that will be used during the build::

   chdir c_stor_build
   cmake [OPT] ../stor/bindings/c
   
.. note::
     OPT refers to the various CMake options you can use, such as:
      - `-G "Unix Makefiles` 
      - `-G "MinGW Makefiles"`
     
     This is the place where to use option `-DSTOR_CRYPTO=ON`
     if you plan on using the encrypting facilities of the database.

Then build the shared library::

   cmake --build . --target cstor
   
and use it in conjunction with the headers in **stor/bindings/c/include/cstor**

And run the tests::

   cmake --build . --target cstor_test
   ./stor_test
   
