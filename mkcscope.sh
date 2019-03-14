#!/bin/sh

BASE=$(dirname $0)

cat /dev/null > filelist.txt
find $BASE/src/ -name "*.hpp" -o -name "*.cpp" -o -name "*.h" >> filelist.txt

rm -f cscope.* tags
cscope -bq -i filelist.txt
rm filelist.txt

#ctags -R $BASE/src/

