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
    STR = 5,
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
        print "Node __init__"
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
            raise TypeError("Invalid NodeType")
        return res

    def __del__(self):
        """
        deletes the cnode via cstor API
        """
        self.__cstor.invoke.esft_stor_node_delete(self.__node)
        print "Node __del__"

    def __len__(self):
        """
        Returns the number of elements in a OBJECT or ARRAY node.
        :raises: TypeError if node is not of NodeType OBJECT or ARRAY
        :return: number of elements in composite node OBJECT or ARRAY
        """
        if self.__type == NodeType.OBJECT or self.__type == NodeType.ARRAY:
            return self.__cstor.invoke.esft_stor_node_size(self.__node)
        else:
            raise TypeError("__len__ can only be called on a node of NodeType OBJECT or ARRAY")

    def __getitem__(self, item):
        """
        Return the Node at item @p item.

        If Node is of type OBJECT, then item is expected to be an instance of basestring
        and a KeyError will be raised if the key does not point to an existing member of Node.

        If Node is of type ARRAY, then item is expected to be either an index or a slice object.
        If the index is out of range, an IndexError will be raisd.

        For any other NodeType, a TypeError will be raised
        :param item: item to look up into the Node
        :return: the Node(s) referred to by the item
        """
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        if isinstance(item, slice):
            if self.__type != NodeType.ARRAY:
                raise Exception("Invalid __getitem__ arg: NodeType is not ARRAY")
            print "__getitem__ slice"
            start = item.start if item.start else 0
            stop = item.stop if item.stop else len(self)
            step = item.step if item.step else 1
            res = []
            for i in range(start, stop, step):
                cnode = self.__cstor.invoke.esft_stor_node_array_get(self.__node, c_int(i), byref(err))
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
            cnode = self.__cstor.invoke.esft_stor_node_array_get(self.__node, c_int(item), byref(err))
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
            cnode = self.__cstor.invoke.esft_stor_node_object_get(self.__node, c_char_p(item), byref(err))
            if err.value:
                self.__cstor.invoke.esft_stor_node_delete(cnode)
                str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                raise Exception("Could not access element of node. Error: " + str_err.value)
            return Node(self.__cstor, cnode)
        else:
            raise Exception("Unsupported __getitem__ arg: " + str(item))

    def __setitem__(self, key, value):
        """

        :param key:
        :param value:
        :return:
        """
        print "__setitem__"

    @property
    def value(self):
        """
        Returns the representation of the value of the Node.

        If Node is of NodeType OBJECT or ARRAY, return a dictionary or list respectively.

        For all other NodeType, returns the python type that refers to the relative NodeType
        :return: representation of the value of the Node.
        """
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
            raise TypeError("Invalid NodeType")

    @value.setter
    def value(self, value):
        """
        Sets the value of this Node to @p value and changes to NodeType accordingly

        If the value type does not have a NodeType counterpart, throws an Exception

        :param value: value to set this Node value to
        """
        pass

    def __contains__(self, item):
        """
        Checks whether item is a child of Node of type OBJECT or ARRAY.

        If this Node is not of type OBJECT or ARRAY raise a TypeError
        :param item: item to check for
        :return: True of item is in the Node, False if not
        """
        if self.__type == NodeType.OBJECT:
            return self.__cstor.invoke.esft_stor_node_object_has(self.__node, c_char_p(item))
        elif self.__type == NodeType.ARRAY:
            children = self.__getitem__(slice(None, None, None))
            for child in children:
                if item == child:
                    return True
        else:
            raise TypeError("__contains__ can only be invoked on OBJECT or ARRAY Node")

    @property
    def json(self):
        """
        Returns the JSON representation of this Node
        :return: JSON of this Node
        """
        tmp = self.__cstor.invoke.esft_stor_node_json(self.__node)
        res = tmp.value
        self.__cstor.invoke.esft_stor_json_dispose(byref(tmp))
        return res

    @json.setter
    def json(self, value):
        """
        Changes the value of this Node with the value represented by the JSON @p value
        :param value: JSON to change this Node value to
        """
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__cstor.invoke.esft_stor_node_from_json(self.__node, value, byref(err))
        if err:
            raise Exception("Failed updating the node value with the provided JSON")

    def __copy__(self):
        """
        Produces a shallow copy of the Node.
        The copies will all point to the same cnode.
        :return: shallow copy of Node
        """
        n = type(self)()
        n.__dict__.update(self.__dict__)
        return n

    def __deepcopy__(self, memo):
        """
        Produces a deep copy of the Node.
        The copies will point to different cnodes.
        :return: deep copy of Node
        """
        return Document(self.__cstor, self.json())


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

    def __del__(self):
        super(Document, self).__del__()

    def __setup(self, json):
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__doc = self.__cstor.invoke.esft_stor_document_create(c_char_p(json), byref(err))
        if err.value:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Could not intitialize Document. Error: " + str_err.value)
        self.__id = c_char_p(self.__cstor.invoke.esft_stor_document_id(self.__doc)).value
        if not self.__id:
            self.__cstor.invoke.esft_stor_document_delete(self.__doc)
            raise Exception("Document did not return a valid ID")
        cnode = self.__cstor.invoke.esft_stor_document_root(self.__doc, byref(err))
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


if __name__ == "__main__":
    cstor = Cstor.load()

    n = Document(cstor, "[1,2,3]")
    print n.id
    print n.json
    n.json = json.dumps([3, 4, 5])
    print n.id
    print n.json
    print "a" in n
    print "b" in n
    print n[0].json
    print n[:1]
    try:
        print n[1.0]
    except Exception as e:
        print e
    try:
        print n["a"]
    except Exception as e:
        print e

