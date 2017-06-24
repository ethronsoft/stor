import os
import shutil

if __name__ == "__main__":
	root = os.path.dirname(os.path.abspath(__file__))
	cmake_deps = os.path.join(root,"dependencies.cmake")
	for r,d,f in os.walk(root):
		if os.path.split(r)[1] == "bin":
			shutil.rmtree(r,ignore_errors=True)
	if os.path.exists(cmake_deps):
		os.remove(os.path.join(root,"dependencies.cmake"))