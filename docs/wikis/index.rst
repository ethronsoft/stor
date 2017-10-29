.. _index:

.. esft::stor documentation master file, created by
   sphinx-quickstart on Wed Oct 25 16:37:54 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

*****        
STOR
*****
   
..  toctree::
    :maxdepth: 2
    
    Introduction <self>
    installation/cpp
    installation/c
    installation/py
    tutorial/cpp
    tutorial/c
    tutorial/py
    api/cpp/modules
    api/c/modules
    api/py/modules

      

Introduction
============

STOR is a multiplatform server-less*, embeddable** NoSQL Document Store written in C++11.

(*), (**)
Similar to `SQLite <https://www.sqlite.org/>`_, STOR does not operate in a client-server scenario.
Rather, you include it/link to it in your project and it works within a single process. 

The major features are the following:
 - Create/Modify/Delete **JSON Documents**
 - Create Databases that persist the spanning process, or get deleted on destruction.
 - Manage **Collections** of Documents
 - **Index** Documents by their fields
 - Query Documents by their IDs
 - **Query** Documents by indexed fields, with **MongoDB-like** query syntax.
 - **Back-up/Restore** Databases.
 - Optionally **encrypt** a Database, by compiling with OpenSSL support and providing a key.
 - **C Bindings**
 - **Python Bindings** + **REPL**

License
--------
 Both STOR and the bindings are released with the `BSD-2-Clause License <https://github.com/ethronsoft/stor/blob/master/LICENSE>`_.
 
 
Dependencies and platforms
--------------------------

Platforms targeted are:
 - Windows
 - OSX
 - Linux
 - Android & iOS (in progress; it just needs CMake toolchain adjustments)

Compilers we tested:
  - MinGW v.
  - GNU C++ v. on Ubuntu
  - Clang v. on Mac OSX


Dependencies:
 - `CMake <https://cmake.org/>`_, as the build tool.
 - `LevelDB <https://github.com/google/leveldb>`_, as the underlying key/value storage engine
 - `Boost.UUID <http://www.boost.org/doc/libs/1_64_0/libs/uuid/>`_, to produce unique Documents' ids
 - `RapidJSON <http://rapidjson.org/>`_, as default JSON parser
 - `Catch <https://github.com/philsquared/Catch>`_, for testing.
 - (Optional) `OpenSSL <https://www.openssl.org/>`_, for database encryption
  
.. note::
      All dependencies are provided in the git repository,
      for simplicity of usage and testing. You are free to 
      provide your own if desired. Some guidance on this will
      be provided in the Installation sections of the various
      products. 
      
A warm **THANK YOU** to the developers of the dependencies. Licenses provided in the respective
stor/core/dependencies folders.


Installation
------------

 - :ref:`cpp-inst`
 - :ref:`c-bindings-inst`    
 - :ref:`py-inst`

API Docs
--------


 - `STOR API <_static/api/cpp/html/index.html>`_
 - `C Bindings API <_static/api/c/html/index.html>`_
 - :ref:`py-api`

Tutorials
---------
 
  - :ref:`cpp-tut`
  - :ref:`c-bindings-tut`
  - :ref:`py-tut`

 

 


