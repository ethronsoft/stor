import argparse
import os
import subprocess
import sys
import shutil

def build(root):
	os.chdir(os.path.join(root,"source"))
	subprocess.check_call("mingw32-make.exe libleveldb.a")

def cleanup(root):
	os.chdir(os.path.join(root,"source"))
	subprocess.check_call("mingw32-make.exe clean",shell=True)
	
def finalize(root,install_path):
	bin_dir = os.path.join(install_path,"bin")
	if os.path.exists(bin_dir):
		shutil.rmtree(bin_dir, ignore_errors=True)
	os.makedirs(bin_dir)
	shutil.copy(os.path.join(root,"source","libleveldb.a"),bin_dir)

if __name__=="__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--install-path",metavar="IP", type=str, required=True,
						help="path to install location")
	args = vars(parser.parse_args())					
	root = os.path.dirname(os.path.abspath(__file__))
	install_path = os.path.abspath(args["install_path"])

	os.environ['TARGET_OS'] = 'NATIVE_WINDOWS'
	try:
		build(root)
		finalize(root,install_path)
	except Exception as ex:
		print ex
	finally:
		cleanup(root)
