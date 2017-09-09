from document import *

class Collection(object):
    """
    A Collection exposes the interface of ccollection.
    Collections are the components of a store
    and they are composed by documents
    A collection allows to perform CRUD operations
    over the documents it owns
    """

    def __init__(self, cstor_functs, ccollection):
        self.__cstor = cstor_functs
        self.__name = None
        self.__ccoll = ccollection
        self.__setup()

    def __del__(self):
        self.__cstor.invoke.esft_stor_collection_delete(self.__ccoll)

    def __setup(self):
        self.__name = self.__cstor.invoke.esft_stor_collection_name(self.__ccoll)

    @property
    def name(self):
        return self.__name

    def add_index(self, index_path):
        if not isinstance(index_path, basestring):
            raise TypeError("add_index expects an index_path string")
        return self.__cstor.invoke.esft_stor_collection_index_add(self.__ccoll, index_path)

    def remove_indices(self):
        return self.__cstor.invoke.esft_stor_collection_index_remove_all(self.__ccoll)

    def put(self, doc):
        if not isinstance(doc, Document):
            raise TypeError("put expects a Document instance")
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        self.__cstor.invoke.esft_stor_collection_document_put(self.__ccoll, doc._Document__doc, byref(err))
        if err.value:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Failed putting provided document into the collection. Error: " + str_err.value)

    def __getitem__(self, doc_id):
        if doc_id not in self:
            raise KeyError("Document with id " + doc_id + " is not in the collection: " + self.name)
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        cdoc = self.__cstor.invoke.esft_stor_collection_document_get(self.__ccoll, c_char_p(doc_id), byref(err))
        return Document.from_cdocument(self.__cstor,cdoc)

    def __contains__(self, doc_id):
        if not isinstance(doc_id, basestring):
            raise TypeError("__getitem__ expects a string")
        return self.__cstor.invoke.esft_stor_collection_document_exists(self.__ccoll, c_char_p(doc_id))

    def remove(self, doc_id):
        return self.__cstor.invoke.esft_stor_collection_document_remove(self.__ccoll, c_char_p(doc_id))

    def find(self, query_string):
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
        self.__cstor.invoke.esft_stor_collection_query_result_delete(res_p, res_len)
        return res

