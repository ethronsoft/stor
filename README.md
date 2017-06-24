# esft::stor
C++ serverless NoSQL document-store 

Platforms:
*Windows - (tested with MinGW)
*OSX
*Linux (tested on Ubuntu 1x.xx)

Tool Dependencies:
 - Cmake (version >=3.6)
 - We CMake "MinGW Makefiles" generator, with gcc/g++ (tested on 5.1.0 but all C++11 fully-compliant versions should work).
 On Windows, the compilation/linking tools used are those provided by TDM-GCC 64bit
 (http://tdm-gcc.tdragon.net/). You may use what you prefer but know that this is 
 what we used.
 - (Optional - for Windows). Some of the commands issued during building may require
 unix-tools. The usage of a compatible shell environment 
 (we use the one provided by https://git-for-windows.github.io/) should suffice to 
 build from command line. Alternatively (or in conjuction with the above), we use
 UnxUtils (http://unxutils.sourceforge.net/).
 

Library Dependencies:
 - Catch        (tested on version 1.7.2) -- https://github.com/philsquared/Catch
 - Rapidjson    (tested on version 1.0.2) -- https://github.com/miloyip/rapidjson
 - Leveldb      (tested on version 1.18)  -- https://github.com/google/leveldb
 - Boost(UUID)  (tested on version 1.55)  -- http://www.boost.org/

All dependencies licenses can be found on the links above or in the "license" folder
in the respective folder under dependencies/. 

  Note:
    All dependencies are already provided (for convenience) 
    in the dependencies/ folder for the respective
    platforms. 
    Building scripts called during CMake-builds automatically take care
    of building the respective 64-bit libraries for you. You are free to manage
    the dependencies the way you like, just make sure you modify the Cmake accordingly.
    Boost UUID dependencies, extracted with the bcp utility (http://www.boost.org/doc/libs/1_64_0/tools/bcp/doc/html/index.html)
    are also included. If you already have Boost headers on the PATH g++ looks into for headers, then you may remove
    it from the dependencies/build.py and remove boost_uuid_ROOT_DIR from CmakeLists.txt
