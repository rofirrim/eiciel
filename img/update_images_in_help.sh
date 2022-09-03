#!/bin/bash

for l in ../doc/*/figures;
do
  echo "Updating lang '$l'"
  for i in "$l"/*;
  do
    if [ -e $(basename $i) ];
    then
      echo "  Updating $i..."
      cp -f $(basename $i) $i
    fi
  done
  echo "Done updating lang '$l'"
done
