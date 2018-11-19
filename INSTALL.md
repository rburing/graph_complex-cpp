# Installing `graph_complex-cpp`

Here are some hints to get these programs running.

## Prerequisites

Using Debian or similar:

    apt-get install libcln-dev libginac-dev nauty libnauty2-dev

Alternatively, install these from source as described below.
If you want to install them locally, use a `--prefix` as indicated.
If you don't use a `--prefix`, you can ignore `PKG_CONFIG_PATH`.

Install nauty:

    tar xvzf nauty26r11.tar.gz
    cd nauty26r11
    ./configure
    make 

Install CLN:

    git clone git://www.ginac.de/cln.git
    cd cln
    autoreconf -i
    ./configure --prefix=$HOME
    make
    make check
    make install

Install GiNaC:

    git clone git://www.ginac.de/ginac.git
    cd ginac
    autoreconf -i
    export PKG_CONFIG_PATH="$HOME/lib/pkgconfig" # for bash
    setenv PKG_CONFIG_PATH "$HOME/lib/pkgconfig" # for tcsh
    ./configure --prefix=$HOME
    make
    make check
    make install

Adjust `LD_LIBRARY_PATH` (in case of local installations):

- In case of bash, add to `~/.bashrc`:
  `export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$HOME/lib"`

- In case of tcsh, add to `~/.tcshrc`:
  `setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${HOME}/lib"`

## Building

Adjust the Makefile (in case of local installations):

- Add `-I${HOME}/include` to `CFLAGS`

- Add `-L${HOME}/lib` to `GINAC_LDFLAGS`

- Add paths to nauty in `NAUTY_CFLAGS` and `NAUTY_LDFLAGS`

Adjust `NAUTY_DIR` in `src/generate_graphs.py`

Depending on the versions of CLN, GiNaC, and your compiler,
you may want to remove `-Werror` and such from `CFLAGS`.

Build as usual:

    make
