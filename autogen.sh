#!/bin/sh

libtoolize --copy --force

rm -f config.cache config.log

aclocal && autoheader && autoconf && automake --add-missing && echo "ok"
