#!/bin/bash

# Assumes your prefix is /opt/gtk4,
# Note the lib layout is the one I got on Debian amd64, other distributions
# (like Fedora) might be using lib64.

export PATH=/opt/gtk4/bin:${PATH}
export PKG_CONFIG_PATH=/opt/gtk4/lib/pkgconfig:/opt/gtk4/lib/x86_64-linux-gnu/pkgconfig:/opt/gtk4/share/pkgconfig
export LDFLAGS=-L/opt/gtk4/lib/x86_64-linux-gnu
export LD_LIBRARY_PATH=/opt/gtk4/lib:/opt/gtk4/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}
export XDG_DATA_DIRS=/opt/gtk4/share:${XDG_DATA_DIRS}
