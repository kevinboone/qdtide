#!/bin/bash
# Of interest only to Kevin :)
WEB=/home/kevin/docs/kzone5/
SOURCE=$WEB/source
TARGET=$WEB/target
make clean
(cd ..; tar cvfz $TARGET/qdtide.tar.gz qdtide)
cp README_qdtide.html $SOURCE
(cd $WEB; ./make.pl qdtide)
