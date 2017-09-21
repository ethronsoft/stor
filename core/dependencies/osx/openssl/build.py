import sys
import os
import argparse
import shutil
import subprocess

def build(root, install_path):
	os.chdir(os.path.join(root,"source"))
	subprocess.check_call("/bin/sh ./Configure no-shared no-asm darwin64-x86_64-cc", shell=True)
	subprocess.check_call("make build_libs", shell=True)

def finalize(root, install_path):
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
	open(os.path.join(install_path,".built"), "w")

def clean(root):
	os.chdir(os.path.join(root,"source"))
	subprocess.call("make clean", shell=True)
	

if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("-i","--install-path",type=str,required=True,default=".")
	args = vars(parser.parse_args())
	root = os.path.dirname(os.path.abspath(__file__))
	install_path = os.path.abspath(args["install_path"])
	try:
		build(root, install_path)
		finalize(root, install_path)
	except Exception as e:
		print('Error on line {}'.format(sys.exc_info()[-1].tb_lineno), type(e), e)
	finally:
		clean(root)
