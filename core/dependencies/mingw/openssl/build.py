import argparse
import os
import subprocess
import sys
import shutil

def build(root):
	os.chdir(os.path.join(root,"source"))
	subprocess.check_call("perl Configure mingw64 no-shared no-asm")
	subprocess.check_call("mingw32-make.exe build_libs", shell=True)

def clean(root):
	os.chdir(os.path.join(root,"source"))
	subprocess.check_call("mingw32-make.exe clean",shell=True)

def finalize(root,install_path):
	bin_dir = os.path.join(install_path,"bin")
	inc_dir = os.path.join(install_path,"include")
	if os.path.exists(inc_dir):
		shutil.rmtree(inc_dir,ignore_errors=True)
	if os.path.exists(bin_dir):
		shutil.rmtree(bin_dir,ignore_errors=True)
	os.makedirs(bin_dir)
	
	shutil.copy(os.path.join(root,"source","libssl.a"),bin_dir)
	shutil.copy(os.path.join(root,"source","libcrypto.a"),bin_dir)
	shutil.copytree(os.path.join(root,"source","include"), inc_dir)

if __name__=="__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--install-path",metavar="IP", type=str, required=True,
						help="path to install location")
	args = vars(parser.parse_args())					
	root = os.path.dirname(os.path.abspath(__file__))
	install_path = os.path.abspath(args["install_path"])
	try:
		build(root)
		finalize(root,install_path)
	except Exception as ex:
		print ex
	finally:
		clean(root)
                