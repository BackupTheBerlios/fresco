#!/bin/sh

aclocal
autoheader
automake --add-missing --verbose
autoconf
