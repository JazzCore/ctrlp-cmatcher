# CtrlP C matching extension

This is a [ctrlp.vim](https://github.com/kien/ctrlp.vim) extension which can be used to get different matching algorithm, written in C language with a small portion of Python (only to access C module).

This extension uses an adapted version of [CommandT](https://github.com/wincent/Command-T) matching, big thanks to @wincent!

## Advantages
- Matcher, written in C can provide significant speed improvement when working on large projects with 10000+ files, e.g [Metasploit](https://github.com/rapid7/metasploit-framework). Dont forget to set ``g:ctrlp_max_files`` option to 0 or 10000+ if you're working on such projects.
- In some cases you can get more precise matching results ( e.g. when trying to match exact file name like ``exe.rb``)

## Drawbacks

There no real drawbacks, but i need to point out some things that may not work as you expected:

- ``Regex`` mode doesnt use C matching and probably will never use it. If you will use it with this extension it will fall back to ``ctrlp.vim`` matching and may be slow on large projects.

## Installation

1. Get extension files with your favorite method. Example for Vundle:

    ```vim
    Bundle 'JazzCore/ctrlp-cmatcher'
    ```
2. Compile C extension.
  * On Linux:

      First, get Python header files. Example for Debian/Ubuntu:

      ```bash
      [sudo] apt-get install python-dev
      ```

      Then compile C module and install it as a Python module:

      ```bash
      cd ~/.vim/bundle/ctrlp-cmatcher/autoload/
      [sudo] python setup.py install
      ```
  * On Windows:

      Installation is similar to Linux version, but it can be more complicated becase of weird errors during compilation.

      First of all, make sure that you have ``python`` in your ``%PATH%``.

      Also you will need MinGW compiler suite installed. Dont forget to add ``C:\MinGW\bin`` to your ``%PATH%``.

      Then go to ``ctrlp-cmatcher\autoload`` dir and run:

      ```bash
      python setup.py build --compiler=mingw32
      python setup.py install
      ```

      If you are getting __gcc: error: unrecognized command line option '-mno-cygwin'__ error, follow [this fix](http://stackoverflow.com/questions/6034390/compiling-with-cython-and-mingw-produces-gcc-error-unrecognized-command-line-o).

3. Edit your ``.vimrc``:

  Add following line:

  ```vim
  let g:ctrlp_match_func = {'match' : 'matcher#cmatch' }
  ```

4. All done!
