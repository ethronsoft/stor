.. _py-inst:

Python Bindings Installation
============================


PYSTOR uses CSTOR (see :ref:`c-bindings-inst`) as the interface to esft::stor via `CTypes <https://docs.python.org/2/library/ctypes.html>`_ 
so the shared library **libcstor has to be built**.

First, get the repository::

    git clone https://github.com/ethronsoft/stor
    
The files of interest to build the Python version of STOR are in the
**stor/bindings/python** and **stor/bindings/c** folders.
    
Proceed with building libcstor, with an out of source build::
    
    
    mkdir py_stor_build
    chdir py_stor_build
    cmake [OPT] ../stor/bindings/python
    
.. note::
     OPT refers to the various CMake options you can use, such as:
      - `-G "Unix Makefiles` 
      - `-G "MinGW Makefiles"`
     
     This is the place where to use option `-DSTOR_CRYPTO=ON`
     if you plan on using the encrypting facilities of the database.

And proceed with "setting up" the python project::

    cmake --build . --target pystor_setup

.. note::
    **Alternatively**, you can do::
        
        python stor/bindings/python/build.py
        
    which will do the above building and setup for you.  
    Consult::
    
        python stor/bindings/python/build.py --help 
  
What this setup is doing is simply copy the shared library cstor into
the python project source, so that it may be packaged with it via `setup.py`

After setup, the python package can be installed normally::

    chdir ../stor/bindings/python
    python setup.py install 

.. note::
    The above command is going to expose a command line entry-point called "pystor".
    You can invoke it from the command like to get in a REPL environment, where you may
    interact with STOR databases using the PYSTOR implementation.
    
After setup (and possibly installation if you don't have the required packages),
you may run tests like so::

    chdir ../stor/bindinds/python
    python setup.py test


    