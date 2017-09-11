from collection import *

def esft_default_encrypt_failure_cb():
    print "Failed encrypting DB"

class Store(object):
    """
    A Store exposes the interface of ccstor.
    A Store is made of collections.
    A store allows to perform CRUD operations
    over the collections it owns
    """

    def __init__(self, cstor_functs, db_home, db_name, temp = False,
                 password=None, encrypt_failure_cb = esft_default_encrypt_failure_cb):
        self.__cstor = cstor_functs
        self.__home = None
        self.__db = None
        self.__setup(db_home=db_home,db_name=db_name,temp=temp,password=password, enc_cb=encrypt_failure_cb)

    def __del__(self):
        """
        deletes cstore via cstor API
        :return: 
        """
        self.__cstor.invoke.esft_stor_store_delete(self.__db)

    def __setup(self, **kwargs):
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        if not kwargs["password"]:
            self.__db = self.__cstor.invoke.esft_stor_store_open(c_char_p(kwargs["db_home"]),
                                                                 c_char_p(kwargs["db_name"]),
                                                                 c_bool(kwargs["temp"]),
                                                                 byref(err))
            if err.value:
                str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                raise Exception("Failed intializing db {0} at {1}. Error: {2}".format(kwargs["db_name"],
                                                                                      kwargs["db_home"],str_err.value))
        else:
            enc_callback = ENCRYPT_CALLBACK(kwargs["enc_cb"])
            self.__db = self.__cstor.invoke.esft_stor_store_open_encrypted(c_char_p(kwargs["db_home"]),
                                                                 c_char_p(kwargs["db_name"]),
                                                                 c_char_p(kwargs["password"]),
                                                                 enc_callback,
                                                                 byref(err))
            if err.value:
                str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
                raise Exception("Failed intializing encrypted db {0} at {1}. Error: {2}".format(kwargs["db_name"],
                                                                                                kwargs["db_home"],
                                                                                                str_err.value))
        self.__home = self.__cstor.invoke.esft_stor_store_home(self.__db)

    @property
    def home(self):
        """
        Returns the path where the store files are allocated
        :return: path of store
        """
        return self.__home

    @property
    def async(self):
        """
        Checks whether the store is in async mode of not. 
        
        Async operations are not guaranteed to be written
        to file on completion but they complete
        more quickly in normal circumstances.
        Particularly useful if client is only writing to the
        store and no synchronization is therefore needed when
        reading.
        
        :return: True if in async mode, False if not
        """
        return self.__cstor.invoke.esft_stor_store_is_async(self.__db)

    @async.setter
    def async(self, value):
        """
        Sets the async moded of the store. value=True turns Async mode on,
        value=False turns Sync mode on.
        :param value: True for Async mode, False for Sync Mode
        """
        if not isinstance(value, bool):
            raise TypeError("bool expected")
        self.__cstor.invoke.esft_stor_store_async(self.__db, c_bool(value))

    def __getitem__(self, collection_name):
        """
        Returns/Creates collection with name @p collection_name
        :param collection_name: name of the Collection to return/create
        :return: Collection with name @p collection_name
        """
        if not isinstance(collection_name, basestring):
            raise TypeError("expected name of a collection as a string")
        err = c_int(self.__cstor.invoke.esft_stor_error_init())
        ccoll = self.__cstor.invoke.esft_stor_store_collection(self.__db, c_char_p(collection_name), byref(err))
        if err.value:
            str_err = c_char_p(self.__cstor.invoke.esft_stor_error_string(err))
            raise Exception("Failed getting collection with name {0}. Error: {1}".format(collection_name, str_err.value))
        return Collection(self.__cstor, ccoll)

    def __contains__(self, collection_name):
        """
        Checks whether a Collection with name @p collection_name exists in the store
        :return: True if Collection with name @p colleciton_name exists, False if not
        """
        if not isinstance(collection_name, basestring):
            raise TypeError("expected name of a collection as a string")
        return self.__cstor.invoke.esft_stor_store_collection_exists(self.__db, c_char_p(collection_name))

    def remove(self, collection_name):
        """
        Removes the Collection @p collection_name from the store if it exists.
        The removed Collection object must no longer be used.
        :param collection_name: name of Collection to remove
        :return: True if succeded, False if not
        """
        if not isinstance(collection_name, basestring):
            raise TypeError("expected name of a collection as a string")
        return self.__cstor.invoke.esft_stor_store_collection_remove(self.__db, c_char_p(collection_name))
