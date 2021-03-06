#!/bin/sh

echo "unarchiving libmp3splt required libs ..."
tar jxf ../libmp3splt_mingw_required_libs.tar.bz2 -C / || exit 1
echo "unarchiving mp3splt-gtk required libs ..."
tar jxf ../mp3splt-gtk_mingw_required_libs.tar.bz2 -C / || exit 1
echo "unarchiving mp3splt-gtk runtime ..."
tar jxf ../mp3splt-gtk_runtime.tar.bz2 || exit 1

if ! grep -q "Generated by" /lib/libiconv.la;then
  sed -i "s/# The name that/\n\
# libogg\.la - a libtool library file\n\
# Generated by ltmain.sh - GNU libtool 5.5.6 \(1.1220.2.95 2004\/04\/11 05:50:42\) Debian: 224 \$\n\
#\n\
# Please DO NOT delete this file!\n\
# It is necessary for linking the library\.\n\
\n\
# The name that/" /lib/libiconv.la
fi

cp -a ./mp3splt-gtk_runtime/*.dll /usr/bin
mkdir -p /lib/.libs
cp /lib/libvorbis* /lib/libmad* /lib/libid3tag* /lib/.libs
cp /bin/libvorbis* /bin/libmad* /bin/libid3tag* /lib/.libs

export PKG_CONFIG_PATH="/usr/lib/pkgconfig"

make -f Makefile.win32 || exit 1;
make -f Makefile.win32 dist || exit 1

