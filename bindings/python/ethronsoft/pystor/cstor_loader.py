from ctypes import *


class esft_cstor_json_struct(Structure):
    _fields_ = [
        ("len", c_int),
        ("value", c_char_p)
    ]



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
    def load(klass):
        libcstor = cdll.LoadLibrary("libcstor")
        return klass(libcstor)

    def __setup__(self):
        self.invoke.esft_stor_node_json.restype = esft_cstor_json_struct
