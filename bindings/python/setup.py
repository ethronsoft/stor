from setuptools import  setup

setup(
    name="pystor",
    version="0.9.0",
    author="Ethronsoft",
    author_email='dev@ethronsoft.com',
    zip_safe=False,
    packages=["ethronsoft", "ethronsoft.pystor"],
    license=open("LICENSE.txt").read(),
    include_package_data=True,
    keywords="nosql document store serverless embedded",
    url="https://github.com/ethronsoft/stor",
    description="Python bindings to esft::stor, a C++ NoSQL serverless document store",
    install_requires=[
        'enum34'
    ],
    setup_requires=[
        'pytest-runner'
    ],
    tests_require=[
        'pytest'
    ],
    entry_points={
        'console_scripts':[
            "pystor = ethronsoft.pystor.__main__:main"
        ]
    }
)
