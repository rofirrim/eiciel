[![C/C++ CI Meson Fedora 37](https://github.com/rofirrim/eiciel/actions/workflows/meson-fedora.yml/badge.svg)](https://github.com/rofirrim/eiciel/actions/workflows/meson-fedora.yml)

# About

Eiciel is a plugin for Nautilus to graphically edit ACL and extended attributes
of files and directories.

# FAQ

1. How is "eiciel" pronounced ?

Just like you would pronounce ACL in English :)

2. What happened to gtk3 version?

Use an earlier release (or the `gtk3` branch) of eiciel when using GNOME 42 or
earlier. This branch only supports GNOME 43 onwards.

3. First steps after you have cloned the repository

You will need Meson 0.57 or later installed (it can be installed using `pip`)
and then follow these steps after you have cloned the repository.

```
$ meson setup builddir
$ cd builddir
$ meson compile
$ meson install
```

4. I don't want the plugin support in GNOME Files

The GNOME Files plugin support is optional, pass `-Dnautilus-plugin=disabled`
to `meson setup` above, to disable it.
