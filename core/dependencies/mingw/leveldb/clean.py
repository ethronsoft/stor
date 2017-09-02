import os
import shutil
import subprocess

if __name__ == "__main__":
    root = os.path.dirname(os.path.abspath(__file__))
    shutil.rmtree(os.path.join(root,"bin"), ignore_errors=True)
    shutil.rmtree(os.path.join(root,"include"), ignore_errors=True)
    if os.path.exists(os.path.join(root,".built")):
        os.remove(os.path.join(root,".built"))