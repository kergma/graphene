#!/bin/sh

[ -f hacks/glx/Makefile.in.orig ] || cp -p hacks/glx/Makefile.in hacks/glx/Makefile.in.orig
cp -p hacks/glx/Makefile.in.orig hacks/glx/Makefile.in
patch -p0 hacks/glx/Makefile.in < hacks/glx/Makefile.in.diff

[ -f driver/XScreenSaver.ad.in ] || cp -p driver/XScreenSaver.ad.in driver/XScreenSaver.ad.in.orig
cp -p driver/XScreenSaver.ad.in.orig driver/XScreenSaver.ad.in
patch -p0 driver/XScreenSaver.ad.in < driver/XScreenSaver.ad.in.diff
