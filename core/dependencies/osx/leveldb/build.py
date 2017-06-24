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
	shutil.copy(os.path.join(root,"source","out-static","libleveldb.a"),install_path)

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
