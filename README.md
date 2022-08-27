[![C/C++ CI Meson](https://github.com/rofirrim/eiciel/actions/workflows/meson.yml/badge.svg)](https://github.com/rofirrim/eiciel/actions/workflows/meson.yml)

# About

Eiciel is a plugin for Nautilus to graphically edit ACL and extended attributes
of files and directories.

# FAQ

1. How is "eiciel" pronounced ?

Just like you would pronounce ACL in English :)

2. What happened to gtk3 version?

Use an earlier release of eiciel when using GNOME 42 or earlier. This branch
only supports GNOME 43 onwards.

3. First steps after you have cloned the repository

You will need Meson 0.54 or later installed (it can be installed using `pip`)
and then follow these steps after you have cloned the repository.

```
$ meson setup builddir
$ cd builddir
$ meson compile
$ meson install
```
