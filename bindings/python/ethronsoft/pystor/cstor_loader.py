import os
import glob
from ctypes import *



class esft_cstor_json_struct(Structure):
    _fields_ = [
        ("len", c_uint),
        ("value", c_char_p)
    ]

class esft_cstor_node_keys(Structure):
    _fields = [
        # ("len", c_uint),
        # ("values", POINTER(c_char_p))
    ]

    def __init__(self, sz):
        self._fields = [
            ("len", c_uint),
            ("values", sz * c_char_p)
        ]

ENCRYPT_CALLBACK = CFUNCTYPE(None)



class Cstor(object):
    """
    internal class that sets up the cstor shared library
    loaded by ctypes with appropriate argument/return types
    and exposes it via the invoke variable for semantic reasons
    """
    def __init__(self, libcstor):
        self.invoke = libcstor
        self.__setup__()

    @classmethod
    def load_from_path(cls, libcstor_path):
        libcstor = cdll.LoadLibrary(libcstor_path)
        return cls(libcstor)

    @classmethod
    def load(cls):
        up = lambda x: os.path.dirname(x)
        root_dir = up(os.path.abspath(__file__))
        deps_dir = os.path.join(root_dir, "bin")
        dep_name = ""
        if os.path.exists(deps_dir):
            m = glob.glob(os.path.join(deps_dir,"libcstor*"))
            if m:
                dep_name = m[0]
        if dep_name:
            libcstor = cdll.LoadLibrary(dep_name)
        else:
            libcstor = cdll.LoadLibrary("libcstor")

        return cls(libcstor)

    def __setup__(self):
        #misc
        self.invoke.esft_stor_error_init.restype = c_int
        self.invoke.esft_stor_error_string.restype = c_char_p

        self.invoke.esft_stor_error_string.argtypes = [c_int]


        #document/node
        self.invoke.esft_stor_document_create.restype = c_void_p
        self.invoke.esft_stor_document_id.restype = c_char_p
        self.invoke.esft_stor_document_root.restype = c_void_p
        self.invoke.esft_stor_node_json.restype = esft_cstor_json_struct
        self.invoke.esft_stor_node_object_keys.restype = POINTER(c_char_p)
        self.invoke.esft_stor_node_object_has.restype = c_bool
        self.invoke.esft_stor_node_is_null.restype = c_bool
        self.invoke.esft_stor_node_as_int.restype = c_int
        self.invoke.esft_stor_node_as_long.restype = c_longlong
        self.invoke.esft_stor_node_as_double.restype = c_double
        self.invoke.esft_stor_node_as_bool.restype = c_bool
        self.invoke.esft_stor_node_as_string.restype = c_char_p
        self.invoke.esft_stor_node_is_int.restype = c_bool
        self.invoke.esft_stor_node_is_long.restype = c_bool
        self.invoke.esft_stor_node_is_double.restype = c_bool
        self.invoke.esft_stor_node_is_bool.restype = c_bool
        self.invoke.esft_stor_node_is_string.restype = c_bool
        self.invoke.esft_stor_node_is_object.restype = c_bool
        self.invoke.esft_stor_node_object_get.restype = c_void_p
        self.invoke.esft_stor_node_is_array.restype = c_bool
        self.invoke.esft_stor_node_array_get.restype = c_void_p
        self.invoke.esft_stor_node_size.restype = c_uint
        
        self.invoke.esft_stor_document_create.argtypes = [c_char_p, POINTER(c_int)]
        self.invoke.esft_stor_document_id.argtypes = [c_void_p]
        self.invoke.esft_stor_document_root.argtypes = [c_void_p, POINTER(c_int)]
        self.invoke.esft_stor_document_delete.argtypes = [c_void_p]
        self.invoke.esft_stor_node_object_keys.argtypes = [c_void_p, POINTER(c_uint)]
        self.invoke.esft_stor_node_object_keys_delete.argtypes = [POINTER(c_char_p), c_uint]
        self.invoke.esft_stor_node_from_json.argtypes = [c_void_p, c_char_p, POINTER(c_int)]
        self.invoke.esft_stor_json_dispose.argtypes = [POINTER(esft_cstor_json_struct)]
        self.invoke.esft_stor_node_delete.argtypes = [c_void_p]
        self.invoke.esft_stor_node_json.argtypes = [c_void_p]
        self.invoke.esft_stor_node_object_has.argtypes = [c_void_p, c_char_p]
        self.invoke.esft_stor_node_is_null.argtypes = [c_void_p]
        self.invoke.esft_stor_node_as_int.argtypes = [c_void_p]
        self.invoke.esft_stor_node_as_long.argtypes = [c_void_p]
        self.invoke.esft_stor_node_as_double.argtypes = [c_void_p]
        self.invoke.esft_stor_node_as_bool.argtypes = [c_void_p]
        self.invoke.esft_stor_node_as_string.argtypes = [c_void_p]
        self.invoke.esft_stor_node_is_int.argtypes = [c_void_p]
        self.invoke.esft_stor_node_is_long.argtypes = [c_void_p]
        self.invoke.esft_stor_node_is_double.argtypes = [c_void_p]
        self.invoke.esft_stor_node_is_bool.argtypes = [c_void_p]
        self.invoke.esft_stor_node_is_string.argtypes = [c_void_p]
        self.invoke.esft_stor_node_is_object.argtypes = [c_void_p]
        self.invoke.esft_stor_node_object_get.argtypes = [c_void_p, c_char_p, POINTER(c_int)]
        self.invoke.esft_stor_node_is_array.argtypes = [c_void_p]
        self.invoke.esft_stor_node_array_get.argtypes = [c_void_p, c_uint, POINTER(c_int)]
        self.invoke.esft_stor_node_size.argtypes = [c_void_p]

        #collection
        self.invoke.esft_stor_collection_name.restype = c_char_p
        self.invoke.esft_stor_collection_index_add.restype = c_bool
        self.invoke.esft_stor_collection_index_remove_all.restype = c_bool
        self.invoke.esft_stor_collection_document_exists.restype = c_bool
        self.invoke.esft_stor_collection_document_remove.restype = c_bool
        self.invoke.esft_stor_collection_query.restype = POINTER(c_void_p)
        self.invoke.esft_stor_collection_index_list.restype = POINTER(c_char_p)
        self.invoke.esft_stor_collection_document_get.restype = c_void_p

        self.invoke.esft_stor_collection_document_remove.argtypes = [c_void_p, c_char_p]
        self.invoke.esft_stor_collection_document_exists.argtypes = [c_void_p, c_char_p]
        self.invoke.esft_stor_collection_index_add.argtypes = [c_void_p, c_char_p]
        self.invoke.esft_stor_collection_name.argtypes = [c_void_p]
        self.invoke.esft_stor_collection_query.argtypes = [c_void_p, c_char_p, POINTER(c_uint), POINTER(c_int)]
        self.invoke.esft_stor_collection_indices_add.argtypes = [c_void_p, POINTER(c_char_p), c_uint]
        self.invoke.esft_stor_collection_index_list.argtypes = [c_void_p, POINTER(c_uint), POINTER(c_int)]
        self.invoke.esft_stor_collection_index_list_delete.argtypes = [POINTER(c_char_p), c_uint]
        self.invoke.esft_stor_collection_document_get.argtypes = [c_void_p, c_char_p, POINTER(c_int)]
        self.invoke.esft_stor_collection_query_result_delete.argtypes = [POINTER(c_void_p), c_uint]

        #store
        self.invoke.esft_stor_store_open.restype = c_void_p
        self.invoke.esft_stor_store_open_encrypted.restype = c_void_p
        self.invoke.esft_stor_store_collection_remove.restype = c_bool
        self.invoke.esft_stor_store_home.restype = c_char_p
        self.invoke.esft_stor_store_is_async.restype = c_bool
        self.invoke.esft_stor_store_collection_exists.restype = c_bool
        self.invoke.esft_stor_store_collection.restype = c_void_p

        self.invoke.esft_stor_store_collection.argtypes = [c_void_p, c_char_p, POINTER(c_int)]
        self.invoke.esft_stor_store_collection_exists.argtypes = [c_void_p, c_char_p]
        self.invoke.esft_stor_store_collection_remove.argtypes = [c_void_p, c_char_p]
        self.invoke.esft_stor_store_is_async.argtypes = [c_void_p]
        self.invoke.esft_stor_store_is_async.argtypes = [c_void_p, c_bool]
        self.invoke.esft_stor_store_home.argtypes = [c_void_p]
        self.invoke.esft_stor_store_open.argtypes = [c_char_p,c_char_p,c_bool,POINTER(c_int)]
        self.invoke.esft_stor_store_open_encrypted.argtypes = [c_char_p, c_char_p,c_char_p,ENCRYPT_CALLBACK,POINTER(c_int)]
        self.invoke.esft_stor_store_delete.argtypes = [c_void_p]