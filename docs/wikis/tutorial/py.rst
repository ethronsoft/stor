.. _py-tut:

Python Bindings Tutorial
========================

The functionality of PYSTOR is equivalent the one described in the :ref:`cpp-tut`, so we will
only show how to use the different API. To better understand what is going on, we recommend 
going through the commentary therein.

.. warning::
    Python Bindings have only been tested on Pyhon 2.7. Future implementations will also target Python 3.6.

.. note::
    PYSTOR uses CTypes to invoke the CSTOR API. The CSTOR API is injected with the helper class Cstor::
    
        Cstor.load(); #will look for the libcstor.[so]|[dll] copied 
                      #by the setup stage into the package.
                      
        Cstor.load_from_path("path/to/libcstor") #loads libcstor.[so]|[dll] from 
                                                 #the user provided path
                                                 
    We will see this object being used to initialize the main PYSTOR classes
    
.. note::
        Examples assume that the following import statement is executed for Document examples::
        
            from ethronsoft.pystor.document import *

        And for Store examples::
        
            from ethronsoft.pystor.store import *
 
Document
--------

The Document is implemented so to behave similarly to a normal dictionary.

Instantiation
^^^^^^^^^^^^^
A Document can be created as an empty JSON::
    
    doc = Document(Cstor.load(), "")
    
a primitive JSON::
    
    doc = Document(Cstor.load(), "5")
    
a JSON array::

    doc = Document(Cstor.load(), "[1,2,3]")
    //or
    doc = Document(Cstor.load(), json.dumps( [1,2,3] ))

a JSON object::
    
    doc = Document(Cstor.load(), "{'a': 1}")
    //or
    doc = Document(Cstor.load(), json.dumps( {"a": 1} ))

Interaction
^^^^^^^^^^^

    Given that Python is not statically typed, there isn't a need to be aware of the Node Type
    as in C++ and C. You can still know what type of Node you have by doing::
    
        node_type = node.type
        
    These types are used internally to call the proper C functions.

Value Extraction
""""""""""""""""

    From a Document (or sub Node), we can get the Python Object relative to the Node Type like so::
    
        py_value = node.value

Navigation
"""""""""" 

    **From an Object Node**, you can navigate the children node, like you would in a dictionary::
    
        for key in node:
            print node[key].value
            
    You can also get the list of keys, like so::
    
        keys = node.keys()
        
    And check if a Key is in the Node, like so::
    
        if "key" in node or "key2" not in node:
            print node["key"].value
        
    **From an Array Node**, you can navigate the member nodes, like you would in a list::
    
        for item in node:
            print item.value
            
        for i in range(len(node)):
            print node[i].value
            
        for item in node[from:to]:
            print item.value
            
    And check in an item is in the Node, like so::
    
        if "Graal" in node or other_item not in node::
            print "Eureka!"
            

Modification
""""""""""""

    You can always assign a Python Object relative to a vlaid Node Type
    and modify the content of the node, like so::
    
        my_obj = {"a": 1}
        my_arr = [1,2,3]
        my_int = 1
        
        node.value = my_obj
        
        node.value = my_arr
        
        node.value = my_int
        
    If you know you are dealing with an Object Node, you can also add a "Key/Value"
    like so::
    
        node["my_key"] = my_value
        
    

Misc
^^^^

    You can get the JSON representation of a Node, like so::
    
        jsn = node.to_json()
        
    Or load a json into a node and change its underlying representation, like so::
    
        node.from_json(json.dumps( [1,2,3] ))
        #which, is equivalent to node.value = [1,2,3]

Store
-----

    

Create a Database
^^^^^^^^^^^^^^^^^

Create/Open a non-encrypted db::

    tmp = tempfile.gettempdir()
    db = Store(Cstor.load(), db_home=tmp, db_name="test_db", temp=True)
    
Create/Open an encrypted db::

    tmp = tempfile.gettempdir()
    db = Store(Cstor.load(), db_home=tmp, db_name="test_db", password="123456789abcdefg")
    

Create/Remove Collections
^^^^^^^^^^^^^^^^^^^^^^^^^

You can create a new Collection (or get an existing one), by the collection name::

    my_collection = db["my_collection"]
    
Or check if a Collection has already been created::

    if "my_collection" in db:
        print "been there, done that"
        
You can remove a collection, like so::

    db.remove(my_collection.name)
    

CRUD Documents in a Collection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Given a document `doc`, we can add it to a Collection, like so::

    my_collection.put(doc)
    
And remove it, like so::

    my_collection.remove(doc.id)
    
We can query a document by its ID::

    doc_id = "some_doc_id"
    doc = my_collection[doc_id]
    
Or check if it has been added to the Collection::

    if doc.id in my_collection:
        print "{} already in".format(doc.id)

Queries
^^^^^^^

Indices
"""""""

To add one index to the Collection::

    my_collection.add_index("some.index.path")
    
To add multiple indices to the Collection::

    my_collection.add_indices(["some.index.path", "other.index.path"])

    
To get an array of current indices::
    
    my_collection.indices()
    
To delete indices::

    my_collection.remove_indices()

Supported Query Operations
""""""""""""""""""""""""""

.. note::
    The syntax of the queries is the same as the one described in :ref:`cpp-tut`
    
Given a valid JSON query string `json_query`, we can get a list of matching Documents, like so::

    matches = my_collection.find(json_query)

Misc
^^^^

Sync/Async
""""""""""

    To check if the database is in "async" write mode::
    
        is_async = db.async
        
    To change the write mode::
    
        db.async(True);//async
        
        db.async(False);//sync
