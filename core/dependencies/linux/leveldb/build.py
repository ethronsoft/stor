import os
import subprocess
import sys
import shutil
import argparse

def build(root):
	os.chdir(os.path.join(root,"source"))
	subprocess.check_call("make clean",shell=True)
	subprocess.check_call("make", shell=True)

def finalize(root, install_path):
	bin_dir = os.path.join(install_path,"bin")
	inc_dir = os.path.join(install_path, "include")
	if os.path.exists(bin_dir):
		shutil.rmtree(bin_dir, ignore_errors=True)
	if os.path.exists(inc_dir):
		shutil.rmtree(inc_dir, ignore_errors=True)
	os.makedirs(bin_dir)
	shutil.copy(os.path.join(root,"source","out-static","libleveldb.a"),bin_dir)
	shutil.copytree(os.path.join(root,"source","include"),inc_dir)
	open(os.path.join(install_path,".built"))

def clean(root):
	os.chdir(os.path.join(root,"source"))
	subprocess.check_call("make clean",shell=True)

if __name__=="__main__":
	root = os.path.dirname(os.path.abspath(__file__))
	parser = argparse.ArgumentParser()
	parser.add_argument("-i","--install-path", type=str, required=True, default=".")
	args = vars(parser.parse_args())
	install_path = os.path.abspath(args["install_path"])
	try:
		build(root)
		finalize(root,install_path)
	except Exception as ex:
		print ex
	finally:
		clean(root)
