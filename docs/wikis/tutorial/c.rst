.. _c-bindings-tut:

C Bindings Tutorial
===================

The functionality of CSTOR is equivalent the one described in the :ref:`cpp-tut`, so we will
only show how to use the different API. To better understand what is going on, we recommend 
going through the commentary therein.


Generally speaking, the **differences boil down to**:

 - Error Reporting as exceptions in C++ and as error codes in C
 - Memory management, automatic in C++ and manual in C.

.. note:: 
    The API headers are in **stor/bindings/c/include/cstor**.
    CSTOR refers to them by path <cstor/.../*.h>, so make sure to add 
    the whole **cstor** folder on a path your compiler looks into for headers. 
    
.. note::
    All the examples assume that the following headers are
    incuded::
        
        //for Documents usage
        #include <cstor/document/cdocument.h>
        
        //for Store usage
        #include <cstor/store/cstore.h>

.. note::
    
    Memory Ownership Strategy:
     - Any API that successfully returns a resource to you, is effectively transferring 
       ownership of that resource to you. Unless the allocation failed and you are returned
       an error code. In that case, any middle-resource created internally is already been cleared
       for you.
       
     - If API asks you to provide a resource, you are not transfering ownership to CSTOR.
       You are still responsible for releasing that resource.
    
    We will provide a list of **ownership-transferring** functions for each section, with their relative
    clean up functions. These functions are also explicitly marked in the API documentation.

.. note::

    The C Bindings API, although functional, does not a cover all the functionalities of the C++ API. 
    Future implementations will likely fill this gap. 
    
Document
--------

+-------------------------------------+-----------------------------------+
| **Ownership-transfering function**  | **Releaser function**             |
+-------------------------------------+-----------------------------------+
| esft_stor_document_create_as_object | esft_stor_document_delete         |
+-------------------------------------+-----------------------------------+
| esft_stor_document_create_as_array  | esft_stor_document_delete         |
+-------------------------------------+-----------------------------------+
| esft_stor_document_create           | esft_stor_document_delete         |
+-------------------------------------+-----------------------------------+
| esft_stor_document_root             | esft_stor_node_delete             |
+-------------------------------------+-----------------------------------+
| esft_stor_node_object_get           | esft_stor_node_delete             |
+-------------------------------------+-----------------------------------+
| esft_stor_node_object_keys          | esft_stor_node_object_keys_delete |
+-------------------------------------+-----------------------------------+
| esft_stor_node_array_get            | esft_stor_node_delete             |
+-------------------------------------+-----------------------------------+
| esft_stor_node_json                 | esft_stor_json_dispose            |
+-------------------------------------+-----------------------------------+

Initialization
^^^^^^^^^^^^^^

Initialize Primitive JSON::

    esft_stor_error_t err = esft_stor_error_init();
    esft_stor_document_t *doc = esft_stor_document_create("5", &err);
    if (err){
        printf("error: %s", esft_stor_error_string(err));
        exit(err);
    }

    esft_stor_document_delete(doc);

Initialize Object JSON::

    esft_stor_error_t err = esft_stor_error_init();
    esft_stor_document_t *doc = esft_stor_document_create("{\"""a\""":1,\"""b\""": 2}", &err);
    //or 
    //esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
    if (err){
        printf("error: %s", esft_stor_error_string(err));
        exit(err);
    }

    esft_stor_document_delete(doc);
    
Initialize Array JSON::

    esft_stor_error_t err = esft_stor_error_init();
    esft_stor_document_t *doc = esft_stor_document_create("[1,2,3]", &err);
    //or 
    //esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
    if (err){
        printf("error: %s", esft_stor_error_string(err));
        exit(err);
    }

    esft_stor_document_delete(doc);
    


Interaction
^^^^^^^^^^^

From a esft_stor_document_t pointer `doc`, we can get the "Node" representation by doing::

    esft_stor_error_t err = esft_stor_error_init();
    esft_stor_node_t *root = esft_stor_document_root(doc, &err); \\ <<-----
    if (err){
        printf("error: %s", esft_stor_error_string(err));
        exit(err);
    }
    
    esft_stor_node_delete(root);

Type Checking
"""""""""""""

    From a esft_stor_node_t pointer `node`, we can check the "type" of Node, like so::
    
        bool is_num    = esft_stor_node_is_num(node);
        bool is_int    = esft_stor_node_is_int(node);
        bool is_long   = esft_stor_node_is_long(node);
        bool is_double = esft_stor_node_is_double(node);
        bool is_bool   = esft_stor_node_is_bool(node);
        bool is_string = esft_stor_node_is_string(node);
        bool is_object = esft_stor_node_is_object(node);
        bool is_array  = esft_stor_node_is_array(node);


Value Extraction
""""""""""""""""

    Knowing the "type" of a esft_stor_node_t, we can extract the value like so::
    
        int value           = esft_stor_node_is_int(node);
        long long value     = esft_stor_node_is_long(node);
        double value        = esft_stor_node_is_double(node);
        bool value          = esft_stor_node_is_bool(node);
        
        //no allocation here, so you may want to copy this string
        //if you want it to survive deallocation of the node.
        const char * value  = esft_stor_node_is_string(node);
        
        //If you expect that the string value may contain a \0,
        //as JSON does not forbid that, then you can use this
        size_t real_len = 0;
        const char * value  = esft_stor_node_as_stringl(node, &real_len);
        
    When extracting an Object Node, or Array Node, we have to clean up after ourselves 
    instead, as we are getting ownership.
    
    So, for extracting from a Object Node, we do::
    
        //check if an object has a key
        if (esft_stor_node_object_has(node, "my_key")){
            esft_stor_error_t err = esft_stor_error_init();
            
            esft_stor_node_t *child = esft_stor_node_object_get(node, "my_key", &err); // <<-----
            if (err){
                printf("error: %s", esft_stor_error_string(err));
                //we should probably not leak `node`
                esft_stor_node_delete(node);
                exit(err);
            }
    
            esft_stor_node_delete(child);
        }
        
    And from an Array Node::
    
        size_t my_index = 1;
        if (my_index < esft_stor_node_size(node)){
            esft_stor_error_t err = esft_stor_error_init();
            
            esft_stor_node_t *child = esft_stor_node_array_get(node, my_index, &err); // <<-----
            if (err){
                printf("error: %s", esft_stor_error_string(err));
                //we should probably not leak `node`
                esft_stor_node_delete(node);
                exit(err);
            }

            esft_stor_node_delete(child);
        }

Navigation
"""""""""" 

If we have an Object Node, we may get the array of children keys, like so::

    size_t keys_len = 0;
    char ** keys = esft_stor_node_object_keys(node, &keys_len);
    
    //No error messages are generated here but, 
    //if `keys_len` gets modified to any value != 0,
    //you will have to release the keys resources
    if (keys_len > 0){
        /** do something with keys, 
            like invoking esft_stor_node_object_get**/
        esft_stor_node_object_keys_delete(keys, key_len);
    }
    

    
If we have an Array Node, we may iterate over the members, like so::

     size_t len = esft_stor_node_size(node);
     
     for (size_t i = 0; i < len; ++i){
        esft_stor_node_t *child = esft_stor_node_array_get(node, my_index, &err); 
        if (err){
            /** handle error **/
        }

        esft_stor_node_delete(child);
     }


Modification
""""""""""""

We can modify the value of a Node by passing it a new JSON representation::

    esft_stor_error_t err = esft_stor_error_init();
    const char * new_json = "5";
    
    
    esft_stor_node_from_json(node, new_json, &err);
    
Or by deep copying the value of another node::

    esft_stor_error_t err = esft_stor_error_init();
    esft_stor_node_t *other_node = /** some other node **/
    
    esft_stor_node_from_node(node, other_node, &err);



    
**If we have an ArrayNode**, then we may add a new node, like so::

    esft_stor_node_array_add_int(node, 1);
    esft_stor_node_array_add_long(node, 5L);
    esft_stor_node_array_add_double(node, 2.0);
    esft_stor_node_array_add_bool(node, true);
    esft_stor_node_array_add_string(node, "hello");
    

**If we have an Object Node**, we may add children, like so::

    esft_stor_node_object_put_int(node,    "key", 1);
    esft_stor_node_object_put_long(node,   "key", 5L);
    esft_stor_node_object_put_double(node, "key", 2.0);
    esft_stor_node_object_put_bool(node,   "key", true);
    esft_stor_node_object_put_string(node, "key", "hello");

    esft_stor_json_dispose


Misc
^^^^

To return a JSON represenation of a Node::

    esft_stor_json_t json = esft_stor_node_json(node);
    
    int json_len            = json.len;
    const char * json_value = json.value;
    
    esft_stor_json_dispose(&json);
    
Store
-----

Create a Database
^^^^^^^^^^^^^^^^^

**Unencrypted database**::

    esft_stor_error_t err = esft_stor_error_init();
    const char *home    = "some/path";
    const char *db_name = "my_db":
    bool remove_on_destruction = false;
    
    esft_stor_store_t *db = esft_stor_store_open(home, db_name, remove_on_destruction, &err);
    if (err){
        /** handle error **/
    }

    esft_stor_store_delete(db);
    
**Encrypted database**.

If the encryption fails, you can be notified via a callback::
    
        void my_failure_callback(){
            printf("failed encryption");
        }

::
   
    esft_stor_error_t err = esft_stor_error_init();
    const char *home    = "some/path";
    const char *db_name = "my_db":
    const char *my_key  = "123456789abcdefg";//16 len string
    bool remove_on_destruction = false;
    
    esft_stor_store_t *db = esft_stor_store_open_encrypted(home, db_name, my_key,
                                                           my_failure_callback, &err);
    if (err){
        /** handle error **/
    }

    esft_stor_store_delete(db);

Create/Remove Collections
^^^^^^^^^^^^^^^^^^^^^^^^^

    To create/Open a Collection::
    
        esft_stor_error_t err = esft_stor_error_init();
        
        esft_stor_collection_t *my_collection = esft_stor_store_collection(db, "my_collection", &err);
        
        if (err){
            /** handle error **/
        }

        esft_stor_collection_delete(my_collection);
        
    To check for the existence of a Collection::
    
        bool exists = esft_stor_store_collection_exists(db, "my_collection");
        
    To remove a Collection::
    
        bool removed = esft_stor_store_collection_remove(db, "my_collection");
        

CRUD Documents in a Collection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can *upsert* a esft_stor_document_t pointer 
(update it if it exists, or insert it if it doesn"t), like so::

    esft_stor_error_t err = esft_stor_error_init();
    
    esft_stor_collection_document_put(my_collection, my_doc, &err);
    if (err){
        printf("failed inserting document: %s", esft_stor_error_string(err))
    }
    

You can remove it, like so::

    const char * doc_id = esft_stor_document_id(my_doc);
    
    bool removed = esft_stor_collection_document_remove(my_collection, doc_id);
    
    
Or check for a document existence::

    const char * doc_id = esft_stor_document_id(my_doc);
    
    bool exists = esft_stor_collection_document_exists(my_collection, doc_id);
    
You can also get a document by its ID::

    esft_stor_error_t err = esft_stor_error_init();
    const char * doc_id = "some_document_id";

    esft_stor_document_t * doc = esft_stor_collection_document_get(my_collection, doc_id, &err);
    if (err){
        /**handle error**/
    }

    esft_stor_document_delete(doc);

Queries
^^^^^^^

Indices
"""""""

To add one index to the Collection::

    bool added = esft_stor_collection_index_add(my_collection, "some.index.path");
    
To add multiple indices to the Collection::

    
    const char * indices[]{
        "some.index.path",
        "another.index.path"
    };
    size_t indices_len = sizeof(indices)/sizeof(char *);
    
    esft_stor_collection_indices_add(my_collection, indices, indices_len);
    
    
To get an array of current indices::
    
    esft_stor_error_t err = esft_stor_error_init();
    size_t len = 0;
    char **indices = esft_stor_collection_index_list(my_collection, &len, &err);
    if (err){
        /** handle error **/
    }
    
    esft_stor_collection_index_list_delete(indices, len);
    
To delete indices::

    esft_stor_collection_index_remove_all(my_collection);

Supported Query Operations
""""""""""""""""""""""""""

.. note::
    The syntax of the queries is the same as the one described in :ref:`cpp-tut`
    
To get an array of Documents related to a valid JSON query `json_query`::

    esft_stor_error_t err = esft_stor_error_init();
    
    size_t len = 0;
    
    esft_stor_document_t* *answer = esft_stor_collection_query(my_collection, json_query, &len, &err);
    if (err){
        /** handle error **/
    }
    
    esft_stor_collection_query_result_delete(answer, len);


Misc
^^^^

Sync/Async
""""""""""

    To check if the database is in "async" write mode::
    
        bool is_async = esft_stor_store_is_async(db);
        
    To change the write mode::
    
        esft_stor_store_async(db, true);//async
        
        esft_stor_store_async(db, false);//sync

    
        