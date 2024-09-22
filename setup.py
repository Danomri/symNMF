from setuptools import Extension, setup

module = Extension("mysymnmf", sources=['symnmf.c', 'symnmfmodule.c'])
setup(name='mysymnmf',
     version='1.1.0',
     author='Dan Omri',
     description= 'Python wrapper for custom C extension',
     ext_modules=[module])
