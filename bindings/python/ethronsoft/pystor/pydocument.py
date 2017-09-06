from ctypes import *
import json
from enum import Enum


class __esft_cstor_json_struct(Structure):
    _fields_ = [
        ("len", c_int),
        ("value", c_char_p)
    ]


class __cstor_helper(object):
    def __init__(self, cstor):
        self.__cstor = cstor
        self.__setup__()

    def invoke(self):
        return self.__cstor

    def __setup__(self):
        self.__cstor.esft_stor_node_json.restype = __esft_cstor_json_struct


class NodeType(Enum):
    OBJECT = 1,
    ARRAY = 2,
    INT = 3,
    LONG = 4,
    STR = 5,
    FLOAT = 6,
    BOOL = 7,
    NULL = 8


class Node(object):
    def __init__(self, cstor_functs, cstor_node):
        self.__cstor = cstor_functs
        self.__node = cstor_node
        self.__type = self.__get_ctype(self.__node)

    def __get_ctype(self, cnode):
        res = None
        if self.__cstor.invoke.esft_stor_node_is_object(cnode):
            res = NodeType.OBJECT
        elif self.__cstor.invoke.esft_stor_node_is_array(cnode):
            res = NodeType.ARRAY
        elif self.__cstor.invoke.esft_stor_node_is_int(cnode):
            res = NodeType.INT
        elif self.__cstor.invoke.esft_stor_node_is_long(cnode):
            res = NodeType.LONG
        elif self.__cstor.invoke.esft_stor_node_is_double(cnode):
            res = NodeType.FLOAT
        elif self.__cstor.invoke.esft_stor_node_is_bool(cnode):
            res = NodeType.BOOL
        elif self.__cstor.invoke.esft_stor_node_is_string(cnode):
            res = NodeType.STR
        elif self.__cstor.invoke.esft_stor_node_is_null(cnode):
            res = NodeType.NULL
        else:
            raise Exception("Could not infer NodeType from cnode")
        return res
    
    def __get_ptype(self, val):
        res = None
        if isinstance(val, dict):
            res = NodeType.OBJECT
        elif isinstance(val, list):
            res = NodeType.ARRAY
        elif isinstance(val, int):
            res = NodeType.INT
        elif isinstance(val, long):
            res = NodeType.LONG
        elif isinstance(val, float):
            res = NodeType.FLOAT
        elif isinstance(val, bool):
            res = NodeType.BOOL
        elif isinstance(val, basestring):
            res = NodeType.STR
        elif val is None:
            res = NodeType.NULL
        else:
            raise Exception("Invalid NodeType")
        return res

    def __del__(self):
        self.__cstor.invoke.esft_stor_node_delete(self.__node)

    def __len__(self):
        if self.__type == NodeType.OBJECT or self.__type == NodeType.ARRAY:
            return self.__cstor.invoke.esft_stor_node_size(self.__node).value
        else:
            raise Exception("__len__ can only be called on a node of NodeType OBJECT or ARRAY")

    def __getitem__(self, item):
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        if isinstance(item, slice):
            if self.__type != NodeType.ARRAY:
                raise Exception("Invalid __getitem__ arg: NodeType is not ARRAY")
            print "__getitem__ slice"
            start = item[0] if item[0] else 0
            stop = item[1] if item[1] else len(self)
            step = item[2] if item[2] else 1
            res = []
            for i in range(start, stop, step):
                cnode = self.__cstor.invoke.esft_stor_node_array_get(self.__node, c_int(i), byref(err))
                if err.value:
                    self.__cstor.invoke.esft_stor_node_delete(cnode)
                    str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                    raise Exception("Could not get root node of Document. Error: " + str_err.value)
                res.append(Node(self.__cstor, cnode))
            return res
        elif isinstance(item, int):
            if self.__type != NodeType.ARRAY:
                raise Exception("Invalid __getitem__ arg: NodeType is not ARRAY")
            if item >= len(self):
                raise IndexError("Array Node index out of range: " + str(item))
            cnode = self.__cstor.invoke.esft_stor_node_array_get(self.__node, c_int(item), byref(err))
            if err.value:
                self.__cstor.invoke.esft_stor_node_delete(cnode)
                str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                raise Exception("Could not access element of node. Error: " + str_err.value)
            return Node(self.__cstor, cnode)
        elif isinstance(item, basestring):
            if self.__type != NodeType.OBJECT:
                raise Exception("Invalid __getitem__ arg: NodeType is not OBJECT")
            if item not in self:
                raise KeyError("Object Node does not have key: " + item)
            cnode = self.__cstor.invoke.esft_stor_node_object_get(self.__node, c_char_p(item), byref(err))
            if err.value:
                self.__cstor.invoke.esft_stor_node_delete(cnode)
                str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                raise Exception("Could not access element of node. Error: " + str_err.value)
            return Node(self.__cstor, cnode)
        else:
            raise Exception("Unsupported __getitem__ arg: " + item)

    def __setitem__(self, key, value):
        print "__setitem__"

    @property
    def value(self):
        if self.__type == NodeType.OBJECT or self.__type == NodeType.ARRAY:
            return json.loads(self.json())
        elif self.__type == NodeType.NULL:
            return None
        elif self.__type == NodeType.INT:
            return self.__cstor.invoke.esft_stor_node_as_int(self.__node).value
        elif self.__type == NodeType.LONG:
            return self.__cstor.invoke.esft_stor_node_as_long(self.__node).value
        elif self.__type == NodeType.FLOAT:
            return self.__cstor.invoke.esft_stor_node_as_double(self.__node).value
        elif self.__type == NodeType.BOOL:
            return self.__cstor.invoke.esft_stor_node_as_bool(self.__node).value
        elif self.__type == NodeType.STR:
            return self.__cstor.invoke.esft_stor_node_as_string(self.__node).value
        else:
            raise Exception("Invalid NodeType")

    @value.setter
    def value(self, value):
        pass

    def __contains__(self, item):
        if self.__type == NodeType.OBJECT:
            return self.__cstor.invoke.esft_stor_node_object_has(self.__node, c_char_p(item))
        elif self.__type == NodeType.ARRAY:
            children = self.__getitem__(self, slice(None, None, None))
            for child in children:
                if item == child:
                    return True
        return False

    @property
    def json(self):
        tmp = self.__cstor.invoke.esft_stor_node_json(self.__node)
        res = tmp.value
        self.__cstor.invoke.esft_stor_json_dispose(byref(tmp))
        return res

    @json.setter
    def json(self, value):
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__cstor.invoke.esft_stor_node_from_json(self.__node, value, byref(err))
        if err:
            raise Exception("Failed updating the node value with the provided JSON")

    def __copy__(self):
        n = type(self)()
        n.__dict__.update(self.__dict__)
        return n

    def __deepcopy__(self, memo):
        return Document(self.__cstor, self.json())


class Document(Node):
    def __init__(self, cstor_functs, json):
        self.__cstor = cstor_functs
        self.__doc = None
        self.__id = None
        self.__get_type(json)

    def __get_type(self, json):
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__doc = self.__cstor.invoke.esft_stor_document_create(c_char_p(json), byref(err))
        if err.value:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Could not intitialize Document. Error: " + str_err.value)
        self.__id = self.__cstor.invoke.esft_stor_document_id(self.__doc)
        if not self.__id:
            raise Exception("Document did not return a valid ID")
        cnode = self.__cstor.invoke.esft_stor_document_root(self.__doc, byref(err))
        if err.value:
            self.__cstor.invoke.esft_stor_node_delete(cnode)
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Could not get root node of Document. Error: " + str_err.value)
        super(Document, self).__init__(self.__cstor, cnode)

    @property
    def id(self):
        return self.__id


def run():
    cstor = cdll.LoadLibrary("libcstor")

    err = c_int(cstor.esft_stor_error_init())

    obj = {"a": 1, "b": {"c": [1, 2, 3]}}

    doc = c_void_p(cstor.esft_stor_document_create(json.dumps(obj), byref(err)))

    doc_id = c_char_p(cstor.esft_stor_document_id(doc))

    root = c_void_p(cstor.esft_stor_document_root(doc, byref(err)))

    cstor.esft_stor_node_json.restype = json_struct
    root_json = cstor.esft_stor_node_json(root)

    print doc_id.value

    print root_json.len
    print root_json.value

    cstor.esft_stor_node_delete(root)
    cstor.esft_stor_document_delete(doc)


if __name__ == "__main__":
    cstor = cdll.LoadLibrary("libcstor")

    n = Node()
    print "a" in n
    print "b" in n
    print n[0]
    print n[:1]
    try:
        print n[1.0]
    except Exception as e:
        print e
    print n["a"]
