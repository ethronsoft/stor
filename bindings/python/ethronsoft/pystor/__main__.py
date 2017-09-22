import pkg_resources
import sys
import code
import readline
from document import *
from store import *

def main():
    vars = globals().copy()
    vars.update(locals())
    version = pkg_resources.require("pystor")[0].version
    banner = """Pystor - {0} on {1}

url: {2}

For general help, invoke help()
For help on Pystor, invoke help("store")
                           help("collection")
                           help("document")
or refer to the wiki found at the above url
""".format(version, sys.platform, "https://github.com/ethronsoft/stor")
    code.interact(banner=banner,local=vars)

if __name__ == "__main__":
    main()
