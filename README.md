# CtrlP C matching extension

This a C++ version with Boost.Python.

To build on Windows:

1. Get boost.
2. `cd C:/boost153`
3. Build Boost.Python static library:
    ```bash
    bjam toolset=msvc link=static threading=multi release stage --with-python
    ```
4. Make a VS project. Name of VS project must patch a name from C++ source ( fuzzycomt ). add `C:\boost153` and `C:\Python27\include` to includes and `C:\boost153\stage\lib` and `C:\Python27\libs` to libraries paths. Switch `General->Configuration Type` to dll and output extension to pyd.


Some notes. To link statically with boost.python on Windows .cpp source file must contain a `#define BOOST_PYTHON_STATIC_LIB ` line. Without it, there will be errors like `unresolved external` and it will complain about not finding `boost_python` lib.

To link dynamically, need to create a dynamic lib with `bjam toolset=msvc link=shared threading=multi release stage --with-python`, remove line about static linking. After building, need to place a boost_python dll to folder with a `*.pyd` file.

It is possible to compile with gcc, but it supports only dynamic linking. For that, need to do a bjam with toolset=gcc and then compile with [this build script](https://gist.github.com/JazzCore/9da53e3ecc803a940172).

If getting errors about undefined external with a boost libs, it could mean that it tries to link a static-built library (link=static in bjam) with a VS project which doesnt define `#define BOOST_PYTHON_STATIC_LIB`.
