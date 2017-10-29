from document import *

class Collection(object):
    """
    A Collection exposes the interface of ccollection.
    Collections are the components of a store
    and they are composed by documents.
    A collection allows to perform CRUD operations
    over the documents it owns
    """

    def __init__(self, cstor_functs, ccollection):
        self.__cstor = cstor_functs
        self.__name = None
        self.__ccoll = ccollection
        self.__setup()

    def __del__(self):
        """
        deletes ccollection via cstor API
        """
        self.__cstor.invoke.esft_stor_collection_delete(self.__ccoll)

    def __setup(self):
        self.__name = c_char_p(self.__cstor.invoke.esft_stor_collection_name(self.__ccoll)).value

    @property
    def name(self):
        """
        returns the name of the collection
        :return: name of the collection
        """
        return self.__name

    def add_index(self, index_path):
        """
        adds an index_path to the collection.
        Index paths allow the collection to be query-able,
        via collection.find, by that index path.

        **index_path syntax**
        
        Given a document with JSON::
        
            {
              'a': {'b' :1}
            }
        
        the index path to query the value 1 would be 'a.b'

        :return: True if operation had effect, False if not
        """
        if not isinstance(index_path, basestring):
            raise TypeError("add_index expects an index_path string")
        return self.__cstor.invoke.esft_stor_collection_index_add(self.__ccoll, c_char_p(index_path))

    def add_indices(self, index_paths):
        """
        adds all the indices provided in interable ``index_paths``
        """
        index_arr_type = len(index_paths) * c_char_p
        index_arr = index_arr_type()
        for i in range(len(index_paths)):
            if not isinstance(index_paths[i], basestring):
                raise TypeError("add_index expects an index_path string")
            index_arr[i] = index_paths[i]
        self.__cstor.invoke.esft_stor_collection_indices_add(self.__ccoll,
                                                             index_arr,
                                                             c_uint(len(index_paths)))

    def indices(self):
        """
        Retrieves all the indices registered in this Collection
        :return: Collection's indices
        """
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        res_len = c_uint(0)
        res_p = self.__cstor.invoke.esft_stor_collection_index_list(self.__ccoll,
                                                               byref(res_len),
                                                               byref(err))
        if err.value:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Failed retrieving indices. Error: " + str_err.value)
        res = [res_p[i] for i in range(res_len.value)]
        self.__cstor.invoke.esft_stor_collection_index_list_delete(res_p, res_len)
        return res

    def remove_indices(self):
        """
        Removes all indices from the Collection
        :return: True if operation had effect, False if not
        """
        return self.__cstor.invoke.esft_stor_collection_index_remove_all(self.__ccoll)

    def put(self, doc):
        """
        Inserts/Replaces Document ``doc`` into the Collection
        :param doc: document to insert/replace
        """
        if not isinstance(doc, Document):
            raise TypeError("put expects a Document instance")
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__cstor.invoke.esft_stor_collection_document_put(self.__ccoll, doc._Document__doc, byref(err))
        if err.value:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Failed putting provided document into the collection. Error: " + str_err.value)

    def __getitem__(self, doc_id):
        """
        Returns the Document with id ``doc_id`` from the Collection. If the Document
        does is not in the Collection, raise KeyError.
        :param doc_id: Document id of the Document to return
        :raise: KeyError if doc_id not in the collection
        :return: Document with id ``doc_id``
        """
        if doc_id not in self:
            raise KeyError("Document with id " + doc_id + " is not in the collection: " + self.name)
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        cdoc = c_void_p(self.__cstor.invoke.esft_stor_collection_document_get(self.__ccoll, c_char_p(doc_id), byref(err)))
        return Document.from_cdocument(self.__cstor,cdoc)

    def __contains__(self, doc_id):
        """
        Checks whether a Document with id ``doc_id`` is in the Collection
        :param doc_id: Document id of the Document to check for existence
        :return: True if Document with id ``doc_id`` exists, False if not
        """
        if not isinstance(doc_id, basestring):
            raise TypeError("__getitem__ expects a string")
        return c_bool(self.__cstor.invoke.esft_stor_collection_document_exists(self.__ccoll, c_char_p(doc_id))).value

    def remove(self, doc_id):
        """
        Removes the Document (if it exists) with id ``doc_id`` from the Collection
        :param doc_id: id of the Document to remove
        :return: True if operation had effect, False if not
        """
        return self.__cstor.invoke.esft_stor_collection_document_remove(self.__ccoll, c_char_p(doc_id))

    def find(self, query_string):
        """
        Executes query ``query_string`` against the database and returns a list of
        Documents that matched the query.

        Query syntax:

        OP can be of type NODE or LEAF::

             { "$NODE-OP": [OP, ...., OP] }
             { "$LEAF-OP": {"INDEX_PATH": "VALUE_AT_INDEX_PATH" }

        LEAF-OPS:
        
         + $eq
         + $neq
         + $gt
         + $gte
         + $lt
         + $lte
         
        NODE-OPS:
        
         + $and
         + $or

        Examples:

        Given::

            Document A
            {
             'a': 1,
             'c': 5,
             'd': 10
            }
            
        and Document B::
        
            {
             'b': 2,
             'c': 5,
             'd': 10
            }

        
        EQUALITY::
        
            >> {'$eq': {'a': 1}}
            matches Document A
            >> {'$eq': {'a': 2}}
            no matches

        NON-EQUALITY::
        
            >> {'$neq': {'a': 2}}
            matches Document A
            >> {'$neq': {'a': 1}}
            no matches

        LESS-THAN::
        
            >> {'$lt': {'a': 2}}
            matches Document A
            >> {'$lt': {'a': 1}}
            no matches

        LESS-EQUAL-THAN::
        
            >> {'$lte': {'a': 2}}
            matches Document A
            >> {'$lte': {'a': 0}}
            no matches

        GREATER-THAN::
        
            >> {'$gt': {'a': 0}}
            matches Document A
            >> {'$gt': {'a': 1}}
            no matches

        GREATER-EQUAL-THAN::
        
            >> {'$gte': {'a': 1}}
            matches Document A
            >> {'$gte': {'a': 2}}
            no matches

        AND::
        
            >> {'$and': [{'$eq': {'c': 5}},
                         {'$eq': {'d': 10}}
                        ]}
            matches Documents A and B
            >> {'$and': [{'$eq': {'a': 1}},
                         {'$eq': {'d': 10}}
                        ]}
            matches Document A
            >> {'$and': [{'$eq': {'a': 5}},
                         {'$eq': {'d': 15}}
                        ]}
            no matches

        OR::
        
            >> {'$or': [{'$eq': {'a': 1}},
                         {'$eq': {'b': 2}}
                        ]}
            matches Documents A and B


        :param query_string:
        :return: list of matched Documents
        """
        if not isinstance(query_string, basestring):
            raise TypeError("query expects a query string")
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        res_len = c_uint(0)
        res_p = self.__cstor.invoke.esft_stor_collection_query(self.__ccoll,
                                                               c_char_p(query_string),
                                                               byref(res_len),
                                                               byref(err))
        if err.value:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Failed executing query. Error: " + str_err.value)
        res = [Document.from_cdocument(self.__cstor, res_p[i]) for i in range(res_len.value)]
        return res

