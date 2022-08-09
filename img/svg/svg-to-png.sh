#!/usr/bin/env bash

# Execute this script from the directory that contains it!

[ -d png ] || mkdir png
rm -vf png/*.png

for i in *.svg;
do
    png_file="png/${i/.svg/.png}"
    inkscape \
    --export-width=24 \
    --export-type=png \
    "--export-filename=${png_file}" "$i"
done
