import argparse
import subprocess
import os
import sys
import shutil

def root():
    return os.path.dirname(os.path.abspath(__file__))

def clean_build_dir(root):
    os.chdir(root)
    shutil.rmtree(os.path.join(root, "_tmp_build"), ignore_errors=True)

def build_binaries(root, with_ssl):
    ssl_opt = "ON" if with_ssl else "OFF"
    build_dir = os.path.join(root, "_tmp_build")
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    os.chdir(build_dir)
    cpp_flags = "-m64"
    c_flags = "-m64"
    generator = "Unix Makefiles" if (sys.platform != "win32") else "MinGW Makefiles"
    subprocess.check_call("cmake -DCMAKE_CXX_FLAGS={0} -DCMAKE_C_FLAGS={1} -G \"{2}\" -DSTOR_CRYPTO={3} ../".
                          format(cpp_flags, c_flags, generator, ssl_opt), shell=True)
    subprocess.check_call("cmake --build . --target pystor_setup", shell=True)

if __name__ == "__main__":
    r = root()
    parser = argparse.ArgumentParser()
    parser.add_argument("-wc", "--with-crypto", action="store_true", required=False,
                        help="build with crypto support")
    args = vars(parser.parse_args())

    try:
        build_binaries(r, args["with_crypto"])
    finally:
        clean_build_dir(r)
