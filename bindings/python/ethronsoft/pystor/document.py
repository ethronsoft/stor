from cstor_loader import *
from enum import Enum
import json

class NodeType(Enum):
    """
    cstor is safely typed.
    This enum lets us reference
    the types supported by cstor
    by name.
    """
    OBJECT = 1,
    ARRAY = 2,
    INT = 3,
    LONG = 4,
    STRING = 5,
    FLOAT = 6,
    BOOL = 7,
    NULL = 8


class Node(object):
    """
    A Node exposes the interface of cnode
    as an object that acts as a dictionary, for
    a OBJECT node, as an array for an ARRAY node
    and as a simple type for all other NodeType(s).
    If operations that require a node to be of a particular
    type are invoked on the wrong node, an Exception may be
    thrown
    """

    def __init__(self, cstor_functs, cstor_node):
        self.__cstor = cstor_functs
        self.__node = cstor_node
        self.__type = self.__get_ctype(self.__node)

    def __get_ctype(self, cnode):
        """
        Returns the node NodeType by invoking the relative
        cstor function on the node cnode
        :param cnode: the cnode related to this Node
        :return: NodeType
        """
        res = None
        if c_bool(self.__cstor.invoke.esft_stor_node_is_object(cnode)).value:
            res = NodeType.OBJECT
        elif c_bool(self.__cstor.invoke.esft_stor_node_is_array(cnode)).value:
            res = NodeType.ARRAY
        elif c_bool(self.__cstor.invoke.esft_stor_node_is_int(cnode)).value:
            res = NodeType.INT
        elif c_bool(self.__cstor.invoke.esft_stor_node_is_long(cnode)).value:
            res = NodeType.LONG
        elif c_bool(self.__cstor.invoke.esft_stor_node_is_double(cnode)).value:
            res = NodeType.FLOAT
        elif c_bool(self.__cstor.invoke.esft_stor_node_is_bool(cnode)).value:
            res = NodeType.BOOL
        elif c_bool(self.__cstor.invoke.esft_stor_node_is_string(cnode)).value:
            res = NodeType.STRING
        elif c_bool(self.__cstor.invoke.esft_stor_node_is_null(cnode)).value:
            res = NodeType.NULL
        else:
            raise Exception("Could not infer NodeType from cnode")
        return res
    
    def __get_ptype(self, val):
        """
        Returns the node NodeType based on the type of value
        :param val: value to use for finding NodeType
        :return: NodeType
        """
        res = None
        if isinstance(val, dict):
            res = NodeType.OBJECT
        elif isinstance(val, list):
            res = NodeType.ARRAY
        elif isinstance(val, bool):
            res = NodeType.BOOL
        elif isinstance(val, int):
            res = NodeType.INT
        elif isinstance(val, long):
            res = NodeType.LONG
        elif isinstance(val, float):
            res = NodeType.FLOAT
        elif isinstance(val, basestring):
            res = NodeType.STRING
        elif val is None:
            res = NodeType.NULL
        else:
            raise TypeError("Invalid NodeType")
        return res

    def __del__(self):
        """
        deletes the cnode via cstor API
        """
        self.__cstor.invoke.esft_stor_node_delete(self.__node)

    def __len__(self):
        """
        Returns the number of elements in a OBJECT or ARRAY node.
        :raises: TypeError if node is not of NodeType OBJECT or ARRAY
        :return: number of elements in composite node OBJECT or ARRAY
        """
        if self.__type == NodeType.OBJECT or self.__type == NodeType.ARRAY:
            return c_uint(self.__cstor.invoke.esft_stor_node_size(self.__node)).value
        else:
            raise TypeError("__len__ can only be called on a node of NodeType OBJECT or ARRAY")

    def __iter__(self):
        if self.__type == NodeType.OBJECT:
            return iter(self.keys())
        elif self.__type == NodeType.ARRAY:
            return iter(self[:])
        elif self.__type == NodeType.STRING:
            return iter(self.value)
        else:
            raise TypeError("__iter__ can only be invoked on OBJECT, ARRAY or STRING Node")

    def keys(self):
        if self.__type != NodeType.OBJECT:
            raise TypeError("keys can only be invoked on OBJECT Node")
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        sz = c_uint(0)
        keys_p = self.__cstor.invoke.esft_stor_node_object_keys(self.__node, byref(sz))
        if sz.value != len(self):
            raise Exception("Failed getting the Node keys")
        res = [keys_p[i] for i in range(sz.value)]
        self.__cstor.invoke.esft_stor_node_object_keys_delete(keys_p, sz)
        return res

    def __getitem__(self, item):
        """
        Return the Node at item @p item.

        If Node is of type OBJECT, then item is expected to be an instance of basestring
        and a KeyError will be raised if the key does not point to an existing member of Node.

        If Node is of type ARRAY, then item is expected to be either an index or a slice object.
        If the index is out of range, an IndexError will be raised.

        For any other NodeType, a TypeError will be raised
        :param item: item to look up into the Node
        :return: the Node(s) referred to by the item
        """
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        if isinstance(item, slice):
            if self.__type != NodeType.ARRAY:
                raise Exception("Invalid __getitem__ arg: NodeType is not ARRAY")
            start = item.start if item.start else 0
            stop = item.stop if item.stop else len(self)
            step = item.step if item.step else 1
            res = []
            for i in range(start, stop, step):
                cnode = c_void_p(self.__cstor.invoke.esft_stor_node_array_get(self.__node, c_uint(i), byref(err)))
                if err.value:
                    self.__cstor.invoke.esft_stor_node_delete(cnode)
                    str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                    raise Exception("Could not access element of node. Error: " + str_err.value)
                res.append(Node(self.__cstor, cnode))
            return res
        elif isinstance(item, int):
            if self.__type != NodeType.ARRAY:
                raise Exception("Invalid __getitem__ arg: NodeType is not ARRAY")
            if item >= len(self):
                raise IndexError("Array Node index out of range: " + str(item))
            cnode = c_void_p(self.__cstor.invoke.esft_stor_node_array_get(self.__node, c_uint(item), byref(err)))
            if err.value:
                self.__cstor.invoke.esft_stor_node_delete(cnode)
                str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                raise Exception("Could not access element of node. Error: " + str_err.value)
            return Node(self.__cstor, cnode)
        elif isinstance(item, basestring):
            if self.__type != NodeType.OBJECT:
                raise TypeError("Invalid __getitem__ arg: NodeType is not OBJECT")
            if item not in self:
                raise KeyError("Object Node does not have key: " + item)
            cnode = c_void_p(self.__cstor.invoke.esft_stor_node_object_get(self.__node, c_char_p(item), byref(err)))
            if err.value:
                self.__cstor.invoke.esft_stor_node_delete(cnode)
                str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                raise Exception("Could not access element of node. Error: " + str_err.value)
            return Node(self.__cstor, cnode)
        else:
            raise Exception("Unsupported __getitem__ arg: " + str(item))

    def __setitem__(self, key, value):
        """
        Sets the Node at member/index @p key to Node resulting from value @p value
        """
        self[key].value = value

    @property
    def type(self):
        """
        Returns the NodeType of this node
        :return: NodeType
        """
        return self.__type

    @property
    def value(self):
        """
        Returns the representation of the value of the Node.

        If Node is of NodeType OBJECT or ARRAY, return a dictionary or list respectively.

        For all other NodeType, returns the python type that refers to the relative NodeType
        :return: representation of the value of the Node.
        """
        if self.__type == NodeType.OBJECT or self.__type == NodeType.ARRAY:
            return json.loads(self.to_json())
        elif self.__type == NodeType.NULL:
            return None
        elif self.__type == NodeType.INT:
            return c_int(self.__cstor.invoke.esft_stor_node_as_int(self.__node)).value
        elif self.__type == NodeType.LONG:
            return c_longlong(self.__cstor.invoke.esft_stor_node_as_long(self.__node)).value
        elif self.__type == NodeType.FLOAT:
            return c_double(self.__cstor.invoke.esft_stor_node_as_double(self.__node)).value
        elif self.__type == NodeType.BOOL:
            return c_bool(self.__cstor.invoke.esft_stor_node_as_bool(self.__node)).value
        elif self.__type == NodeType.STRING:
            return c_char_p(self.__cstor.invoke.esft_stor_node_as_string(self.__node)).value
        else:
            raise TypeError("Invalid NodeType")

    @value.setter
    def value(self, value):
        """
        Sets the value of this Node to @p value and changes to NodeType accordingly

        If the value type does not have a NodeType counterpart, throws an Exception

        :param value: value to set this Node value to
        """
        value_type = self.__get_ptype(value)
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__cstor.invoke.esft_stor_node_from_json(self.__node, c_char_p(json.dumps(value)), byref(err))
        if err:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Could not set node value. Error: " + str_err.value)
        self.__type = value_type

    def __contains__(self, item):
        """
        Checks whether item is a child of Node of type OBJECT or ARRAY.

        If this Node is not of type OBJECT or ARRAY raise a TypeError
        :param item: item to check for
        :return: True of item is in the Node, False if not
        """
        if self.__type == NodeType.OBJECT:
            return isinstance(item, basestring) and \
                   c_bool(self.__cstor.invoke.esft_stor_node_object_has(self.__node, c_char_p(item))).value
        elif self.__type == NodeType.ARRAY:
            for i in range(len(self)):
                if item == self[i].value:
                    return True
        elif self._type == NodeType.STRING:
            return item in self.value
        else:
            raise TypeError("__contains__ can only be invoked on OBJECT, ARRAY or STRING Node")

    def to_json(self):
        """
        Returns the JSON representation of this Node
        :return: JSON of this Node
        """
        tmp = self.__cstor.invoke.esft_stor_node_json(self.__node)
        res = tmp.value
        self.__cstor.invoke.esft_stor_json_dispose(byref(tmp))
        return res

    def from_json(self, value):
        """
        Changes the value of this Node with the value represented by the JSON @p value
        :param value: JSON to change this Node value to
        """
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__cstor.invoke.esft_stor_node_from_json(self.__node, c_char_p(value), byref(err))
        if err:
            raise Exception("Failed updating the node value with the provided JSON")

    def __deepcopy__(self, memo):
        """
        Produces a deep copy of the Node.
        The copies will point to different cnodes.
        :return: deep copy of Node
        """
        return Document(self.__cstor, self.to_json())


class Document(Node):
    """
    A Document exposes the interface of cdocument.
    A Document represents an item of a Collection.
    A Document is the root Node and a client should
    always be creating the root Node using the Document
    class rather than initializing a Node themselves.
    """
    def __init__(self, cstor_functs, json):
        self.__cstor = cstor_functs
        self.__doc = None
        self.__id = None
        self.__setup(json)

    @classmethod
    def from_cdocument(cls, cstor_functs, cdoc):
        """
        Creates a document from an existing cdocument object
        """
        err = c_int(cstor_functs.invoke.esft_stor_error_init())
        cnode = c_void_p(cstor_functs.invoke.esft_stor_document_root(cdoc, byref(err)))
        if err.value:
            cstor_functs.invoke.esft_stor_document_delete(cdoc)
            str_err = c_char_p(cstor_functs.invoke.esft_stor_error_string(err))
            raise Exception("Could not get root node of Document. Error: " + str_err.value)
        cid = c_char_p(cstor_functs.invoke.esft_stor_document_id(cdoc)).value
        if not id:
            cstor_functs.invoke.esft_stor_document_delete(cdoc)
            raise Exception("Document did not return a valid ID")
        self = cls.__new__(cls)
        self.__doc = cdoc
        self.__id = cid
        Node.__init__(self,cstor_functs,cnode)
        return self

    def __del__(self):
        """
        deletes the cdocument via cstor API
        """
        self.__cstor.invoke.esft_stor_document_delete(self.__doc)
        super(Document, self).__del__()

    def __setup(self, json):
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__doc = c_void_p(self.__cstor.invoke.esft_stor_document_create(c_char_p(json), byref(err)))
        if err.value:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Could not intitialize Document. Error: " + str_err.value)
        self.__id = c_char_p(self.__cstor.invoke.esft_stor_document_id(self.__doc)).value
        if not self.__id:
            self.__cstor.invoke.esft_stor_document_delete(self.__doc)
            raise Exception("Document did not return a valid ID")
        cnode = c_void_p(self.__cstor.invoke.esft_stor_document_root(self.__doc, byref(err)))
        if err.value:
            self.__cstor.invoke.esft_stor_document_delete(self.__doc)
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Could not get root node of Document. Error: " + str_err.value)
        super(Document, self).__init__(self.__cstor, cnode)

    @property
    def id(self):
        """
        Returns the UUID of the Document.
        :return: UUID
        """
        return self.__id
