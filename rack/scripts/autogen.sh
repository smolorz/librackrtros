#!/bin/bash

echo -n cleaning up .

# Clean up the generated crud
(
    if test -d config/autoconf; then
	cd config/autoconf
	for FILE in compile config.guess config.sub depcomp install-sh ltmain.sh missing mkinstalldirs; do
	    if test -f $FILE; then
		rm -f $FILE
	    fi
	    echo -n .
	done
    else
	mkdir -p config/autoconf
    fi
)

for FILE in aclocal.m4 configure config/rackrtros_config_pre.h.in; do
    if test -f $FILE; then
	rm -f $FILE
    fi
	echo -n .
done

for DIR in autom4te.cache; do
    if test -d $DIR; then
	rm -rf $DIR
    fi
	echo -n .
done

find . -type f -name 'GNUmakefile.in' -print0 | xargs -r0  rm -f --
find . -type f -name 'GNUmakefile' -print0 | xargs -r0 rm -f --

echo ' done'

if test x"${1}" = x"clean"; then
    exit
fi

aclocal -I config/m4
libtoolize --force --copy
autoheader
automake --add-missing --copy --gnu -Wall -Wno-override -Wno-portability
autoconf -Wall
