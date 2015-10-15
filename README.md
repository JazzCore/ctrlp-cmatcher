# CtrlP C matching extension

This is a [ctrlp.vim][ctrlp] extension which can be used to get different matching algorithm, written in C language with a small portion of Python (only to access C module).

This extension uses an adapted version of [CommandT][commandt] matching, big thanks to @wincent!

## Advantages
- Matcher, written in C, can provide significant speed improvement when working on large projects with 10000+ files, e.g [Metasploit][metasploit]. Dont forget to set ``g:ctrlp_max_files`` option to 0 or 10000+ if you're working on such projects.
- In some cases you can get more precise matching results ( e.g. when trying to match exact file name like ``exe.rb``)

## Drawbacks

There are no real drawbacks, but I need to point out some things that may not work as you expected:

- ``Regex`` mode doesn't use C matching and probably will never use it. If you will use it with this extension it will fall back to ``ctrlp.vim`` matching and may be slow on large projects.

## Installation

1. Get extension files with your favorite method. Example for Vundle:

    ```vim
    Plugin 'JazzCore/ctrlp-cmatcher'
    ```
2. Compile C extension.
  If you are getting any errors on this stage you can try the manual installation guide located [here][manual].

  * On Linux/Unix systems:

      First, get Python header files. Example for Debian/Ubuntu:

      ```sh
      [sudo] apt-get install python-dev
      ```

      Then run the installation script:

      ```sh
      cd ~/.vim/bundle/ctrlp-cmatcher
      ./install.sh
      ```

  * On OS X (tested with 10.9.2 Mavericks):

      First [fix the compiler](http://stackoverflow.com/a/22322645/6962):

      ```sh
      export CFLAGS=-Qunused-arguments
      export CPPFLAGS=-Qunused-arguments
      ```

      Then run the installation script:

      ```sh
      cd ~/.vim/bundle/ctrlp-cmatcher
      ./install.sh
      ```

  * On Windows:

      Installation is similar to Linux version, but it can be more complicated because of weird errors during compilation.

      First of all, make sure that you have ``python`` in your ``%PATH%``.

      Also you will need MinGW compiler suite installed. Dont forget to add ``C:\MinGW\bin`` to your ``%PATH%``.

      Then go to ``ctrlp-cmatcher`` dir and run the installation script:

      ```
      install_windows.bat
      ```

      If you are getting __gcc: error: unrecognized command line option '-mno-cygwin'__ error, follow [this fix](http://stackoverflow.com/questions/6034390/compiling-with-cython-and-mingw-produces-gcc-error-unrecognized-command-line-o).

3. Edit your ``.vimrc``:

  Add following line:

  ```vim
  let g:ctrlp_match_func = {'match' : 'matcher#cmatch' }
  ```

4. All done!

[ctrlp]: https://github.com/kien/ctrlp.vim
[commandt]: https://github.com/wincent/Command-T
[metasploit]: https://github.com/rapid7/metasploit-framework
[manual]: https://github.com/JazzCore/ctrlp-cmatcher/wiki/Manual-installation
