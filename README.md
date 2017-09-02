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

    if compiling with STOR_CRYPTO flag, openSSL will also be built. On the MinGW platform, you are likely to get the most trouble.
	If something goes wrong try to run dependencies/build.py from unix-friendly shell (like git-shell).
	Perl is a requirement to Configure openssl, so make sure you have it. If you are not able to get openssl/build.py to work,
	compile openssl through whatever mean you find and put the following files/folders in the dependencies/mingw/openssl folder:
	- `include` directory that contains the `openssl` folder that contains all openssl headers
	- `bin` folder that contains libssl.a, libcrypto.a 
	- `.built` file to tell the dependencies.build.py that the depedency has been built
    
    When linking libstor.a to your own executable, you may want to take note of the dependencies linked with the `stor_test` target
    for each platform and replicate the linking them in your own executable. 
