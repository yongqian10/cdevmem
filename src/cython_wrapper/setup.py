from setuptools import Extension, setup
from Cython.Build import cythonize
import numpy

examples_extension = Extension(
    name="pycdevmem",
    sources=["pycdevmem.pyx"],
    libraries=["cdevmem"],
    library_dirs=["../../build/"],
    include_dirs=["../cdevmem/", numpy.get_include()]
)
setup(
    name="pycdevmem",
    ext_modules=cythonize([examples_extension], compiler_directives={'language_level':"3"})
)