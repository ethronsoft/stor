import os
import glob
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
    def load_from_path(cls, libcstor_path):
        libcstor = cdll.LoadLibrary(libcstor_path)
        return cls(libcstor)

    @classmethod
    def load(cls):
        up = lambda x: os.path.dirname(x)
        root_dir = up(up(up(os.path.abspath(__file__))))
        deps_dir = os.path.join(root_dir, "deps")
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
        self.invoke.esft_stor_node_json.restype = esft_cstor_json_struct