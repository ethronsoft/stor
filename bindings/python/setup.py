from setuptools import  setup

setup(
    name="pystor",
    version="0.9.0",
    author="Ethronsoft",
    packages=["ethronsoft","ethronsoft/pystor"],
    namespace_packages = ['ethronsoft'],
    license=open("LICENSE.txt").read(),
    install_requires=[
        'enum34'
    ],
    setup_requires=[
        'pytest-runner'
    ],
    tests_require=[
        'pytest'
    ]
)
