#!/bin/bash

#this file creates a netbsd package for mp3splt-gtk

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ../include_variables.sh

echo
echo $'Package :\tnetbsd'
echo

#we generate the makefile
echo "# \$NetBSD\$

DISTNAME=       mp3splt-gtk-${MP3SPLT_GTK_VERSION}
PKGNAME=        mp3splt-gtk-nbsd-${MP3SPLT_GTK_VERSION}
CATEGORIES=     audio
MASTER_SITES=   \${MASTER_SITE_SOURCEFORGE:=mp3splt/}

MAINTAINER=     io_alex_2002@yahoo.fr
HOMEPAGE=       http://mp3splt.sourceforge.net/
COMMENT=        Libmp3splt GTK2 gui to split mp3 and ogg without decoding

PKG_INSTALLATION_TYPES= overwrite pkgviews

CONFIGURE_ARGS+=        --enable-bmp
GNU_CONFIGURE=          YES
USE_TOOLS+=             pkg-config

DEPENDS+=       libmp3splt-nbsd-${LIBMP3SPLT_VERSION}:../../audio/libmp3splt
DEPENDS+=       bmp-[0-9]*:../../audio/bmp
DEPENDS+=       gtk2+>=2.6:../../x11/gtk2
DEPENDS+=       glib2>=2.6:../../devel/glib2

OBJMACHINE=     YES
DOC_DIR=\${PREFIX}/share/doc/mp3splt-gtk/

#copy documentation
pre-install:
        \${INSTALL_DATA_DIR} \${DOC_DIR}" > Makefile

for doc in "${MP3SPLT_GTK_DOC_FILES[@]}";do
    echo "	\${INSTALL_DATA} \${WRKSRC}/${doc} \${DOC_DIR}" >> Makefile
done

echo "
.include \"../../audio/libmp3splt/buildlink3.mk\"
.include \"../../devel/glib2/buildlink3.mk\"
.include \"../../x11/gtk2/buildlink3.mk\"
.include \"../../audio/bmp/buildlink3.mk\"

.include \"../../mk/bsd.pkg.mk\"" >> Makefile

#we generate the PLIST
echo "@comment \$NetBSD\$" > PLIST

for file in "${MP3SPLT_GTK_FILES[@]}";do
    echo "$file" >> PLIST
done

for doc in "${MP3SPLT_GTK_DOC_FILES[@]}";do
    echo "share/doc/mp3splt-gtk/$doc" >> PLIST
done

echo "@dirrm share/doc/mp3splt-gtk" >> PLIST
