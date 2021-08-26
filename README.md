[![C/C++ CI Autotools](https://github.com/rofirrim/eiciel/actions/workflows/autotools.yml/badge.svg)](https://github.com/rofirrim/eiciel/actions/workflows/autotools.yml)
[![C/C++ CI Meson](https://github.com/rofirrim/eiciel/actions/workflows/meson.yml/badge.svg)](https://github.com/rofirrim/eiciel/actions/workflows/meson.yml)

# About

Eiciel is a plugin for Nautilus to graphically edit ACL and extended attributes
of files and directories.

# FAQ

1. How is "eiciel" pronounced ?

Just like you would pronounce ACL in English :)

2. First steps after you have cloned the repository

```
$ autoreconf -vif
$ ./configure
$ make
# make install
```

Eiciel is migrating its build system from
[Autotools](https://en.wikipedia.org/wiki/GNU_Autotools) to
[Meson](https://mesonbuild.com/).

If you want to build using Meson, you will need Meson 0.54 or later
installed (it can be installed using `pip`) and then follow these steps
after you have cloned the repository.

```
$ mkdir build
$ cd build
$ meson ..
$ meson compile
$ meson install
```
