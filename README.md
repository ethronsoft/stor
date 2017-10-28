**For a more complete Documentation**, refer to http://esft-stor.readthedocs.io/en/latest/

**Note**: Pull requests not accepted until February 2018, as this is part of my MSc thesis project.
          
_________

# esft::stor
C++ serverless NoSQL document-store 

Platforms:
 - Windows - (tested with MinGW)
 - OSX
 - Linux (tested on Ubuntu 1x.xx)

Tool Dependencies:
 - Cmake (version >=3.6)
 - On Windows, we used GCC/G++. The compilation/linking tools used are those provided by TDM-GCC 64bit
 (http://tdm-gcc.tdragon.net/). You may use what you prefer but know that this is 
 what we used.
 - (Optional - for Windows). Some of the commands issued during building may require
 unix-tools. The usage of a compatible shell environment 
 (we use the one provided by https://git-for-windows.github.io/) should suffice to 
 build from command line. Alternatively (or in conjuction with the above), we use
 UnxUtils (http://unxutils.sourceforge.net/) + cmd.exe or PowerShell.
 - (Optional) CLion IDE integrates seamlessly with CMake. CodeBlocks should as well. 
 

Library Dependencies:
 - Catch        (tested on version 1.7.2) -- https://github.com/philsquared/Catch
 - Rapidjson    (tested on version 1.0.2) -- https://github.com/miloyip/rapidjson
 - Leveldb      (tested on version 1.18)  -- https://github.com/google/leveldb
 - Boost(UUID)  (tested on version 1.55)  -- http://www.boost.org/
 - [OpenSSL]    (optional, if compiling 
                 with STOR_CRYPTO flag.
                 tested on version 1.0.2) -- https://www.openssl.org/

All dependencies licenses can be found on the links above or in the "license" folder
in the respective folder under dependencies/. 

  Note:
  
    All dependencies are already provided (for convenience and for self-contained testing) 
    in the dependencies/ folder for the respective
    platforms. 
    Building scripts called during CMake-builds automatically take care
    of building the respective 64-bit libraries for you. You are free to manage
    the dependencies the way you like, just make sure you modify the CmakeLists.txt accordingly.
    
    Boost UUID dependencies, extracted with the bcp utility (http://www.boost.org/doc/libs/1_64_0/tools/bcp/doc/html/index.html)
    are also included. If you already have Boost headers on the PATH g++ looks into for headers, then you may remove
    it from the dependencies/build.py and remove boost_uuid_ROOT_DIR from CmakeLists.txt.
    
    Tip: When linking libstor.a to your own executable, you may want to take note of the dependencies linked with the `stor_test` target
    for each platform and replicate the linking them in your own executable. 
    
Install:

esft::stor uses Cmake.
To build the library, make an out of source directory
   
   `mkdir stor_build`

Then setup the project Cmake cache and relative files that will be used during the build

   `chdir stor_build`
   `cmake [OPT] ../stor`
   
   Note:
   
     OPT refers to the various CMake options you can use, such as `-G "Unix Makefiles`. This is the place
     where to use `-DSTOR_CRYPTO` if you plan on using the encrypting facilities of the database.

Then build the desired target

   `cmake --build . --target stor`

And run the tests:

   `cmake --build . --target stor_test`
   `./stor_test`
    
# CSTOR
CSTOR is the C binding of esft::stor

The sources are located in bindings/c

CSTOR uses Cmake like esft::stor so installation follows the same guideline

The test target is `cstor_test` and the shared library target is `cstor`

    Note:
    As with esft::stor, you can specify -DSTOR_CRYPTO if desired. 


# PYSTOR
PYSTOR is the Python binding of esft::stor

The sources are located in bindings/python

Install: 

Pystor uses CSTOR as the interface to esft::stor via `ctypes` (https://docs.python.org/2/library/ctypes.html)
so the shared library libcstor has to be built.

To build libcstor, proceed with setting up cmake as noted above (again, -DSTOR_CRYPTO can be specified)
and then invoke the cmake target `pystor_setup`
Alternatively, the script bindinds/python/build.py may be invoked.

After having setup pystor, the shared library libcstor will be placed in the python package and you can proceed
with invoking the following command

`python setup.py install`

to complete the installation. 

