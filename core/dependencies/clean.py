import os
import shutil
import subprocess

if __name__ == "__main__":
	root = os.path.dirname(os.path.abspath(__file__))
	cmake_deps = os.path.join(root,"dependencies.cmake")
	platforms = ["linux","mingw","osx"]
	deps = ["leveldb","openssl"]
	clean_files = [os.path.join(root,p,d,"clean.py") for p in platforms for d in deps]
	for cf in clean_files:
		if os.path.exists(cf):
			subprocess.check_call("python {0}".format(cf), shell=True)
	if os.path.exists(cmake_deps):
		os.remove(os.path.join(root,"dependencies.cmake"))