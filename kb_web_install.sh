#!/bin/bash
WEB=/home/kevin/docs/kzone5/
SOURCE=$WEB/source
TARGET=$WEB/target
make clean
(cd ..; tar cvfz $TARGET/ebookinfo.tar.gz ebookinfo)
cp README_ebookinfo.html $SOURCE
(cd $WEB; ./make.pl ebookinfo)
