from distutils.core import setup, Extension
import os, platform

if os.name == 'nt' and platform.architecture()[0] == '64bit':
    extSearch = Extension('fuzzycomt', ['fuzzycomt.c'], extra_compile_args=['-std=c99','-D MS_WIN64'])
else:
    extSearch = Extension('fuzzycomt', ['fuzzycomt.c'], extra_compile_args=['-std=c99'])



setup (name = 'fuzzycomt',
       version = '0.1',
       description = 'Fuzzy search in strings',
       ext_modules = [extSearch])

