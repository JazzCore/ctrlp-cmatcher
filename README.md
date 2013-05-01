# CtrlP C matching extension

This a C++ version with Boost.Python.

To build on Windows:

1. Get boost.
2. `cd C:/boost153`
3. Build Boost.Python static library:
    ```bash
    bjam toolset=msvc link=static threading=multi release stage --with-python
    ```
4. Make a VS project. add `C:\boost153` and `C:\Python27\include` to includes and `C:\boost153\stage\lib` and `C:\Python27\libs` to libraries paths. Switch General->Configuration Type to dll and output extension to pyd.
