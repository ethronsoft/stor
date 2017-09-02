import argparse
import shutil
import subprocess
import os

def build_dep(root_path, force=False):
	print root_path
	build_script = os.path.join(root_path,"build.py")
	clean_script = os.path.join(root_path, "clean.py")
	is_built = os.path.join(root_path,".built")
	
	#proceed if a build.py file is found
	if not os.path.exists(build_script): return
	
	#do not re-build if already there or if not forced
	if os.path.exists(is_built):
		if force:
			if os.path.exists(clean_script):
				subprocess.check_call("python {0}".format(clean_script), shell=True)
		else:
			return
	subprocess.check_call("python {0} -i {1}".format(os.path.join(root_path,"build.py"), root_path), shell=True)
	
def cmake_entry(name, path):
		return "set({0}_ROOT_DIR \"{1}\")\n".format(name,path.replace("\\","/"))

if __name__ == "__main__":
	root = os.path.dirname(os.path.abspath(__file__))
	parser = argparse.ArgumentParser()
	parser.add_argument("-f", "--forced", action="store_true", required=False,
						help="force re-build of all dependencies")
	parser.add_argument("-p", "--platform", choices=["mingw","linux","osx"], required=True)
	parser.add_argument("-wssl", "--with-ssl", action="store_true", required=False,
						help="build openssl libraries")
	args = vars(parser.parse_args())
	
	dependencies = {
		"multiplatform": ["catch", "rapidjson", "boost.uuid"],
		args["platform"]: ["leveldb"]
	}
	if args["with_ssl"]: 
		dependencies[args["platform"]].append("openssl")
	
	#cmake dependencies config entry
	entries = []
	for plat in dependencies:
		for dep in dependencies[plat]:
			dep_root = os.path.join(root, plat, dep)
			build_dep(dep_root, args["forced"])
			entries.append(cmake_entry(dep, dep_root))
	
	#write dependencies cmake file
	open(os.path.join(root, "dependencies.cmake"), "w").writelines(entries)
	
		
