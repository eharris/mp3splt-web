#!/bin/bash

################# variables to set ############
#program versions
LIBMP3SPLT_VERSION=0.4_rc1;
MP3SPLT_VERSION=2.2_rc1;
MP3SPLT_GTK_VERSION=0.4_rc1;
ARCH=i386;

#if we upload to sourceforge or not
UPLOAD_TO_SOURCEFORGE=0;

LIBMP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO LIMITS)
MP3SPLT_GTK_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)
MP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)

#if we modify the subversion repository (the ebuild needs renaming)
SUBVERSION=0;
################## end variables to set ############

################## confirmation question ############
#the confirmation question
echo
echo "This script is used by the developers to auto-create packages for releases";
echo "!!!! Warning !!!! This script may be dangerous and erase data on the computer !!";
echo
sleep 3;

select=("I know what I'm doing and I use it at my own risk" "Quit")
select continue in "${select[@]}";do
    if [[ $continue = "Quit" ]]; then
        exit 0;
    else
        break;
    fi;
done;

#don't run the script as root
if [[ `id -u` == 0 ]]; then
    echo "The script must not be run as root"
    exit 1
fi;

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROJECT_DIR=$script_dir/../
cd $PROJECT_DIR
################## end confirmation question ############

################## update_version function ############
# echo "Usage: update_version PROGRAM VERSION LIBMP3SPLT_VERSION"
# echo $PROGRAM can be : libmp3splt, mp3splt or mp3splt-gtk'
function update_version()
{
    #we check the $PROGRAM argument
    if [[ $1 == "libmp3splt" ]] || [[ $1 == "mp3splt" ]] || 
        [[ $1 == "mp3splt-gtk" ]];then
        #we get the program and the version
        PROGRAM=$1;
        VERSION=$2;
        LIBMP3SPLT_VERSION=$3;
        
        #we move in the current script directory
        cd $PROJECT_DIR/scripts
        
        #common changes
        #we do the debian changelog
        if [[ $1 == "mp3splt" ]]; then
            cd ../newmp3splt
        else
            cd ../$PROGRAM
        fi;
        if ! debchange --distribution "testing" -v $VERSION "version "$VERSION 2>/dev/null;then
            rm -f debian/.changelog.dch.swp;
            debchange -r "version "$VERSION;
        fi
        
        #README
        #./README:       libmp3splt version 0.3.1
        sed -i "s/\s*$PROGRAM version.*/\t$PROGRAM version $VERSION/" README;
        #configure.ac
        #./configure.ac:AC_INIT(libmp3splt, 0.3.1, io_alex_2002@yahoo.fr)
        #./configure.ac:AM_INIT_AUTOMAKE(libmp3splt, 0.3.1)
        sed -i "s/AC_INIT($PROGRAM, .*,/\
AC_INIT($PROGRAM, $VERSION,/" ./configure.ac;
        sed -i "s/AM_INIT_AUTOMAKE($PROGRAM, .*)/\
AM_INIT_AUTOMAKE($PROGRAM, $VERSION)/" ./configure.ac;    
        #rpm global Version
        sed -i "s/Version: .*/Version: $VERSION/" ./rpm/SPECS/$PROGRAM.spec
        #arch global version
        sed -i "s/pkgver=.*/pkgver=$VERSION/" ./arch/PKGBUILD
        
        #current date, we need it
        DATE=$(date +%d\\/%m\\/%y);
        NEW_LIBMP3SPLT_VER=${LIBMP3SPLT_VERSION//./_}
        
        case $PROGRAM in
            #libmp3splt settings
            "libmp3splt") 
                #libmp3splt source code
                #./src/mp3splt_types.h:#define SPLT_PACKAGE_VERSION "0.3.1"
                sed -i "s/#define SPLT_PACKAGE_VERSION \".*\"/\
#define SPLT_PACKAGE_VERSION \"$VERSION\"/" ./src/mp3splt_types.h;
                #./src/mp3splt.c:void mp3splt_v0_3_1
                sed -i "s/void mp3splt_v.*/void mp3splt_v$NEW_LIBMP3SPLT_VER()/" ./src/mp3splt.c;
                #./src/Doxyfile:PROJECT_NUMBER=0.3.1
                sed -i "s/PROJECT_NUMBER=.*/PROJECT_NUMBER=$VERSION/" ./src/Doxyfile;
                #update gentoo ebuild
                cd gentoo/media-libs/$PROGRAM
                if [[ $SUBVERSION == 1 ]];then
                    svn mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                    svn ci -m "updated gentoo version" &>/dev/null
                else
                    mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                fi;
                ;;
            #mp3splt settings
            "mp3splt")
                #debian control file, libmp3splt dependency
                #./debian/control:Build-Depends: debhelper (>= 4.0.0), libmp3splt (= 0.3.1)
                #./debian/control:Depends: ${shlibs:Depends}, libmp3splt (= 0.3.1)
                sed -i "s/libmp3splt (= .*)/libmp3splt (= $LIBMP3SPLT_VERSION)/" ./debian/control;
                #windows installer
                #./other/win32_installer.nsi:!define VERSION "2.2.1"
                sed -i "s/!define VERSION \".*\"/!define VERSION \"$VERSION\"/" ./other/win32_installer.nsi;
                #configure.ac libmp3splt version check
                #./configure.ac:AC_CHECK_LIB(mp3splt, mp3splt_v0_3_5,libmp3splt=yes,
                #./configure.ac:        [AC_MSG_ERROR(libmp3splt version 0.3.5 needed :
                sed -i "s/AC_CHECK_LIB(mp3splt, mp3splt_v.*,l/\
AC_CHECK_LIB(mp3splt, mp3splt_v$NEW_LIBMP3SPLT_VER,l/" ./configure.ac;
                sed -i "s/\[AC_MSG_ERROR(libmp3splt version .* needed/\
\[AC_MSG_ERROR(libmp3splt version $LIBMP3SPLT_VERSION needed/" ./configure.ac;
                #source code
                #./src/mp3splt.c:#define VERSION "2.2"
                sed -i "s/#define VERSION \".*\"/#define VERSION \"$VERSION\"/" ./src/mp3splt.c;
                #./src/mp3splt.c:#define MP3SPLT_DATE "14/04/2006"
                sed -i "s/#define MP3SPLT_DATE \".*\"/#define MP3SPLT_DATE \"$DATE\"/" ./src/mp3splt.c;
                #update gentoo ebuild
                cd gentoo/media-sound/$PROGRAM
                if [[ $SUBVERSION == 1 ]];then
                    svn mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                    svn ci -m "updated gentoo version" &>/dev/null
                else
                    mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                fi;
                sed -i "s/media-libs\/libmp3splt-.*/media-libs\/libmp3splt-$LIBMP3SPLT_VERSION/" ./$PROGRAM-$VERSION.ebuild;
                #slackware description
                cd ../../../slackware
                sed -i "s/libmp3splt version .*/libmp3splt version $LIBMP3SPLT_VERSION,/" ./slack-desc
                cd ..
                #rpm libmp3splt Requires
                sed -i "s/libmp3splt = .*/libmp3splt = $LIBMP3SPLT_VERSION/" ./rpm/SPECS/$PROGRAM.spec
                #arch libmp3splt depends
                sed -i "s/libmp3splt=.*'/libmp3splt=${LIBMP3SPLT_VERSION}'/" ./arch/PKGBUILD
                ;;
            #mp3splt-gtk settings
            "mp3splt-gtk")
                #windows installer
                #./other/win32_installer.nsi:!define VERSION "0.3.1"
                sed -i "s/!define VERSION \".*\"/!define VERSION \"$VERSION\"/" ./other/win32_installer.nsi;
                #debian control file, libmp3splt dependency
                #./debian/control:Build-Depends: debhelper (>= 4.0.0), libmp3splt (= 0.3.1), beep-media-player-dev(>= 0.9.7-1)
                #./debian/control:Depends: ${shlibs:Depends}, libmp3splt (= 0.3.1), beep-media-player(>= 0.9.7-1)
                sed -i "s/libmp3splt (= .*)/libmp3splt (= $LIBMP3SPLT_VERSION)/" ./debian/control;
                #configure.ac libmp3splt version check
                #./configure.ac:AC_CHECK_LIB(mp3splt, mp3splt_v0_3_5,libmp3splt=yes,
                #./configure.ac:        [AC_MSG_ERROR(libmp3splt version 0.3.5 needed :
                sed -i "s/AC_CHECK_LIB(mp3splt, mp3splt_v.*,l/\
AC_CHECK_LIB(mp3splt, mp3splt_v$NEW_LIBMP3SPLT_VER,l/" ./configure.ac;
                sed -i "s/\[AC_MSG_ERROR(libmp3splt version .* needed/\
\[AC_MSG_ERROR(libmp3splt version $LIBMP3SPLT_VERSION needed/" ./configure.ac;
                #source code
                #./src/main_win.c:#define VERSION "0.3.1"
                #./src/main_win.c:  g_snprintf(b3, 100, "-release of 27/02/06-\n%s libmp3splt...
                sed -i "s/#define VERSION \".*\"/#define VERSION \"$VERSION\"/" ./src/main_win.c;
                sed -i "s/release of .* libmp3splt/release of $DATE-\\\n%s libmp3splt/" ./src/main_win.c;
                #update gentoo ebuild
                cd gentoo/media-sound/$PROGRAM
                if [[ $SUBVERSION == 1 ]];then
                    svn mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                    svn ci -m "updated gentoo version" &>/dev/null
                else
                    mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                fi;
                sed -i "s/media-libs\/libmp3splt-.*\"/media-libs\/libmp3splt-$LIBMP3SPLT_VERSION\"/" ./$PROGRAM-$VERSION.ebuild;
                #slackware description
                cd ../../../slackware
                sed -i "s/libmp3splt version .*/libmp3splt version $LIBMP3SPLT_VERSION,/" ./slack-desc
                cd ..
                #rpm libmp3splt Requires
                sed -i "s/libmp3splt = .*, b/libmp3splt = $LIBMP3SPLT_VERSION, b/" ./rpm/SPECS/$PROGRAM.spec
                #arch libmp3splt depends
                sed -i "s/libmp3splt=.*'/libmp3splt=${LIBMP3SPLT_VERSION}'/" ./arch/PKGBUILD
                ;;
        esac
    else
        echo "unknown program";
        exit 1;
    fi;
    
    echo "Finished setting up $PROGRAM for version $VERSION with libmp3splt version $LIBMP3SPLT_VERSION";
}
################## end update_version function ############

################## update versions ############
echo
echo "Updating versions..."
echo
sleep 2;

#we update versions
update_version "libmp3splt" $LIBMP3SPLT_VERSION $LIBMP3SPLT_VERSION
update_version "mp3splt-gtk" $MP3SPLT_GTK_VERSION $LIBMP3SPLT_VERSION
update_version "mp3splt" $MP3SPLT_VERSION $LIBMP3SPLT_VERSION

cd $PROJECT_DIR;
################## end update versions ############

#puts .sarge or .etch version
#$1=.sarge or $1=.etch
function put_debian_version()
{
    sed -i "1,4s/libmp3splt (\(.*\))/libmp3splt (\1.$1)/" libmp3splt/debian/changelog
    sed -i "1,4s/mp3splt (\(.*\))/mp3splt (\1.$1)/" newmp3splt/debian/changelog
    sed -i "1,4s/mp3splt-gtk (\(.*\))/mp3splt-gtk (\1.$1)/" mp3splt-gtk/debian/changelog
}

#cleans .sarge or .etch;
#$1=.sarge or $1=.etch
function clean_debian_version()
{
    sed -i "1,4s/libmp3splt (\(.*\).$1)/libmp3splt (\1)/" libmp3splt/debian/changelog
    sed -i "1,4s/mp3splt (\(.*\).$1)/mp3splt (\1)/" newmp3splt/debian/changelog
    sed -i "1,4s/mp3splt-gtk (\(.*\).$1)/mp3splt-gtk (\1)/" mp3splt-gtk/debian/changelog
}

#make the chroot debian flavors
#example : make_debian_flavor "ubuntu" "breezy"
#example : make_debian_flavor "debian" "sarge"
function make_debian_flavor()
{
    echo
    echo "Creating $1 $2 packages..."
    echo
    sleep 2;
    
    put_debian_version "$2"
    dchroot -d -c $1_$2 "export LC_ALL=\"C\" && make && rm -f libmp3splt/*.tar.gz &&\
rm -f newmp3splt/*.tar.gz && rm -f mp3splt-gtk/*.tar.gz" || exit 1
    clean_debian_version "$2"
    cd $PROJECT_DIR;
}

############# source distribution and debian packages ################
echo
echo "Creating source distribution..."
echo
sleep 2;

put_debian_version "etch"
make || exit 1
mv ./mp3splt-gtk/mp3splt-gtk*tar.gz ./
mv ./libmp3splt/libmp3splt*tar.gz ./
mv ./newmp3splt/mp3splt*tar.gz ./
rm -rf $BUILD_TEMP
clean_debian_version "etch"

make_debian_flavor "debian" "sarge"
make_debian_flavor "debian" "sid"
############# end source distribution and debian packages ################

############# ubuntu packages ##########################
make_debian_flavor "ubuntu" "breezy"
make_debian_flavor "ubuntu" "dapper"
make_debian_flavor "ubuntu" "edgy"
############# end ubuntu packages ##########################

############# gnu/linux static build #####
echo
echo "Creating static gnu/linux builds..."
echo
sleep 2;

STATIC_DIR=/tmp/static_tmp;

rm -rf $STATIC_DIR
mkdir -p $STATIC_DIR

#static libmp3splt
LIBMP3SPLT_STATIC_DIR=$STATIC_DIR/libmp3splt
mkdir -p $LIBMP3SPLT_STATIC_DIR/usr/local/share/doc/libmp3splt
cd libmp3splt
./autogen.sh && ./configure --disable-shared --enable-static && make clean && make &&\
make DESTDIR=$LIBMP3SPLT_STATIC_DIR install || exit 1
cp "${LIBMP3SPLT_DOC_FILES[@]}" $LIBMP3SPLT_STATIC_DIR/usr/local/share/doc/libmp3splt
tar -c -z -C $LIBMP3SPLT_STATIC_DIR -f libmp3splt-${LIBMP3SPLT_VERSION}_static_$ARCH.tar.gz .
mv libmp3splt*.tar.gz ..

#we install libmp3splt shared libs too for mp3splt and mp3splt-gtk
#configure scripts
./configure --enable-shared --enable-static && make clean && make &&\
make DESTDIR=$LIBMP3SPLT_STATIC_DIR install || exit 1

#we put the flags for mp3splt and mp3splt-gtk, to find libmp3splt
export CFLAGS="-I$LIBMP3SPLT_STATIC_DIR/usr/local/include"
export LDFLAGS="-L$LIBMP3SPLT_STATIC_DIR/usr/local/lib"

#static mp3splt
MP3SPLT_STATIC_DIR=$STATIC_DIR/mp3splt
mkdir -p $MP3SPLT_STATIC_DIR/usr/local/share/doc/mp3splt
cd ../newmp3splt
./autogen.sh && ./configure --disable-shared --enable-static &&\
make clean && make && make DESTDIR=$MP3SPLT_STATIC_DIR install || exit 1
cp "${MP3SPLT_DOC_FILES[@]}" $MP3SPLT_STATIC_DIR/usr/local/share/doc/mp3splt
tar -c -z -C $MP3SPLT_STATIC_DIR -f mp3splt-${MP3SPLT_VERSION}_static_$ARCH.tar.gz .
mv mp3splt*.tar.gz ..

#static mp3splt-gtk
MP3SPLT_GTK_STATIC_DIR=$STATIC_DIR/mp3splt-gtk
mkdir -p $MP3SPLT_GTK_STATIC_DIR/usr/local/share/doc/mp3splt-gtk
cd ../mp3splt-gtk
./autogen.sh && ./configure --enable-bmp --disable-shared --enable-static && make clean && make &&\
make DESTDIR=$MP3SPLT_GTK_STATIC_DIR install || exit 1
cp "${MP3SPLT_GTK_DOC_FILES[@]}" $MP3SPLT_GTK_STATIC_DIR/usr/local/share/doc/mp3splt-gtk
tar -c -z -C $MP3SPLT_GTK_STATIC_DIR -f mp3splt-gtk-${MP3SPLT_GTK_VERSION}_static_$ARCH.tar.gz .
mv mp3splt-gtk*.tar.gz ..

cd ..
rm -rf $STATIC_DIR
############# end gnu/linux static build #####

############# gnu/linux dynamic build #####
echo
echo "Creating dynamic gnu/linux builds..."
echo
sleep 2;

DYNAMIC_DIR=/tmp/dynamic_tmp;

rm -rf $DYNAMIC_DIR
mkdir -p $DYNAMIC_DIR

#dynamic libmp3splt
LIBMP3SPLT_DYNAMIC_DIR=$DYNAMIC_DIR/libmp3splt
mkdir -p $LIBMP3SPLT_DYNAMIC_DIR/usr/local/share/doc/libmp3splt
cd libmp3splt
./autogen.sh && ./configure --enable-shared --disable-static && make clean && make &&\
make DESTDIR=$LIBMP3SPLT_DYNAMIC_DIR install || exit 1
cp "${LIBMP3SPLT_DOC_FILES[@]}" $LIBMP3SPLT_DYNAMIC_DIR/usr/local/share/doc/libmp3splt
tar -c -z -C $LIBMP3SPLT_DYNAMIC_DIR -f libmp3splt-${LIBMP3SPLT_VERSION}_dynamic_$ARCH.tar.gz .
mv libmp3splt*.tar.gz ..

#we install libmp3splt shared libs too for mp3splt and mp3splt-gtk
#configure scripts
./configure --enable-shared --disable-static && make clean && make &&\
make DESTDIR=$LIBMP3SPLT_DYNAMIC_DIR install || exit 1

#we put the flags for mp3splt and mp3splt-gtk, to find libmp3splt
export CFLAGS="-I$LIBMP3SPLT_DYNAMIC_DIR/usr/local/include"
export LDFLAGS="-L$LIBMP3SPLT_DYNAMIC_DIR/usr/local/lib"

#dynamic mp3splt
MP3SPLT_DYNAMIC_DIR=$DYNAMIC_DIR/mp3splt
mkdir -p $MP3SPLT_DYNAMIC_DIR/usr/local/share/doc/mp3splt
cd ../newmp3splt
./autogen.sh && ./configure --enable-shared --disable-static &&\
make clean && make && make DESTDIR=$MP3SPLT_DYNAMIC_DIR install || exit 1
cp "${MP3SPLT_DOC_FILES[@]}" $MP3SPLT_DYNAMIC_DIR/usr/local/share/doc/mp3splt
tar -c -z -C $MP3SPLT_DYNAMIC_DIR -f mp3splt-${MP3SPLT_VERSION}_dynamic_$ARCH.tar.gz .
mv mp3splt*.tar.gz ..

#dynamic mp3splt-gtk
MP3SPLT_GTK_DYNAMIC_DIR=$DYNAMIC_DIR/mp3splt-gtk
mkdir -p $MP3SPLT_GTK_DYNAMIC_DIR/usr/local/share/doc/mp3splt-gtk
cd ../mp3splt-gtk
./autogen.sh && ./configure --enable-bmp --enable-shared --disable-static && make clean && make &&\
make DESTDIR=$MP3SPLT_GTK_DYNAMIC_DIR install || exit 1
cp "${MP3SPLT_GTK_DOC_FILES[@]}" $MP3SPLT_GTK_DYNAMIC_DIR/usr/local/share/doc/mp3splt-gtk
tar -c -z -C $MP3SPLT_GTK_DYNAMIC_DIR -f mp3splt-gtk-${MP3SPLT_GTK_VERSION}_dynamic_$ARCH.tar.gz .
mv mp3splt-gtk*.tar.gz ..

cd ..
rm -rf $DYNAMIC_DIR
############# end gnu/linux dynamic build #####

############# gentoo ebuilds ################
#we do the ebuilds with gentoo in chroot /mnt/gentoo
echo
echo "Creating gentoo ebuilds..."
echo
sleep 2;

GENTOO_TEMP=/tmp/gentoo_temp;

rm -rf $GENTOO_TEMP
mkdir -p $GENTOO_TEMP

#libmp3splt ebuild
cp -a ./libmp3splt/gentoo/* $GENTOO_TEMP;
find $GENTOO_TEMP -name \".svn\" -exec rm -rf '{}' &>/dev/null \;
#digest libmp3splt
dchroot -d -c gentoo "cp *.tar.gz /usr/portage/distfiles;
ebuild $GENTOO_TEMP/media-libs/libmp3splt/libmp3splt* digest;" || exit 1
tar czf libmp3splt-${LIBMP3SPLT_VERSION}_ebuild.tar.gz $GENTOO_TEMP/media-libs;
rm -rf $GENTOO_TEMP/*;

#mp3splt-gtk ebuild
cp -a ./mp3splt-gtk/gentoo/* $GENTOO_TEMP;
find $GENTOO_TEMP -name \".svn\" -exec rm -rf '{}' &>/dev/null \;
#digest mp3splt-gtk
dchroot -d -c gentoo "ebuild \
$GENTOO_TEMP/media-sound/mp3splt-gtk/mp3splt* digest;" || exit 1
tar czf mp3splt-gtk-${MP3SPLT_GTK_VERSION}_ebuild.tar.gz $GENTOO_TEMP/media-sound;
rm -rf $GENTOO_TEMP/*;

#mp3splt ebuild
cp -a ./newmp3splt/gentoo/* $GENTOO_TEMP;
find $GENTOO_TEMP -name \".svn\" -exec rm -rf '{}' &>/dev/null \;
#digest mp3splt-gtk
dchroot -d -c gentoo "ebuild \
$GENTOO_TEMP/media-sound/mp3splt/mp3splt* digest;" || exit 1
tar czf mp3splt-${MP3SPLT_VERSION}_ebuild.tar.gz $GENTOO_TEMP/media-sound;
rm -rf $GENTOO_TEMP/*;

#end ebuilds temp directory
rm -rf $GENTOO_TEMP;
############# end gentoo ebuilds ################

############# windows installers ################
echo
echo "Creating windows installers..."
echo
sleep 2;

#we do the windows executables
./crosscompile_win32.sh || exit 1
############# end windows installers ################

############# RPM packages creation ################
echo
echo "Creating RPMs..."
echo
sleep 2;

RPM_TEMP=/tmp/rpm_temp

rm -rf $RPM_TEMP
mkdir -p $RPM_TEMP

#libmp3splt
echo "%_topdir $PROJECT_DIR/libmp3splt/rpm" > ~/.rpmmacros
cd libmp3splt && make dist || exit 1
mv libmp3splt*tar.gz ./rpm/SOURCES
cd rpm && rpmbuild -ba ./SPECS/libmp3splt.spec || exit 1
rm -rf ./BUILD/*
rm -rf ./SOURCES/*
mv ./RPMS/$ARCH/*.rpm ../../ || exit 1
mv ./SRPMS/*.rpm ../../ || exit 1

#mp3splt
echo "%_topdir $PROJECT_DIR/newmp3splt/rpm" > ~/.rpmmacros
cd ../../newmp3splt && \
CFLAGS="-I$RPM_TEMP/libmp3splt/usr/include" LDFLAGS="-L$RPM_TEMP/libmp3splt/usr/lib" ./configure && make dist\
 || exit 1
mv mp3splt*tar.gz ./rpm/SOURCES
cd rpm
CFLAGS="-I$RPM_TEMP/libmp3splt/usr/include" LDFLAGS="-L$RPM_TEMP/libmp3splt/usr/lib" \
rpmbuild -ba ./SPECS/mp3splt.spec || exit 1
rm -rf ./BUILD/*
rm -rf ./SOURCES/*
mv ./RPMS/$ARCH/*.rpm ../../ || exit 1
mv ./SRPMS/*.rpm ../../ || exit 1

#mp3splt-gtk
echo "%_topdir $PROJECT_DIR/mp3splt-gtk/rpm" > ~/.rpmmacros
cd ../../mp3splt-gtk && \
CFLAGS="-I$RPM_TEMP/libmp3splt/usr/include" LDFLAGS="-L$RPM_TEMP/libmp3splt/usr/lib" ./configure && make dist\
 || exit 1
mv mp3splt-gtk*tar.gz ./rpm/SOURCES
cd rpm
CFLAGS="-I$RPM_TEMP/libmp3splt/usr/include" LDFLAGS="-L$RPM_TEMP/libmp3splt/usr/lib" \
rpmbuild -ba ./SPECS/mp3splt-gtk.spec || exit 1
rm -rf ./BUILD/*
rm -rf ./SOURCES/*
mv ./RPMS/$ARCH/*.rpm ../../ || exit 1
mv ./SRPMS/*.rpm ../../ || exit 1

rm -rf $RPM_TEMP

cd ../..
############# end RPM packages creation ################

############# archlinux packages #########
echo
echo "Creating archlinux packages..."
echo
sleep 2;

#libmp3splt
cd libmp3splt
./autogen.sh && ./configure && make dist || exit 1
mv libmp3splt*.tar.gz ./arch || exit 1
cd arch
dchroot -d -c arch "makepkg" || exit 1
mv libmp3splt*pkg.tar.gz ../..
rm -rf ./libmp3splt*.tar.gz

#mp3splt
cd ../../newmp3splt
CFLAGS="-I../libmp3splt/arch/pkg/usr/include"
LDFLAGS="-L../libmp3splt/arch/pkg/usr/lib"
./autogen.sh && ./configure && make dist || exit 1
mv mp3splt*.tar.gz ./arch || exit 1
cd arch
dchroot -d -c arch "makepkg -d -c;" || exit 1
mv mp3splt*pkg.tar.gz ../..
rm -rf ./mp3splt*.tar.gz

#mp3splt-gtk
cd ../../mp3splt-gtk
./autogen.sh && ./configure && make dist || exit 1
mv mp3splt-gtk*.tar.gz ./arch || exit 1
cd arch
dchroot -d -c arch "makepkg -d -c;" || exit 1
mv mp3splt-gtk*pkg.tar.gz ../..
rm -rf ./mp3splt-gtk*.tar.gz

#remove remaining libmp3splt build
cd ../../libmp3splt/arch
rm -rf ./pkg ./src && rm -rf ./filelist

cd $PROJECT_DIR
############# end archlinux packages #########

############# openbsd packages #####
cd /mnt/personal/systems/bsd-based/openbsd && ./openbsd
cd $PROJECT_DIR
############# end openbsd packages #####

############# netbsd packages #####
cd /mnt/personal/systems/bsd-based/netbsd && ./netbsd
cd $PROJECT_DIR
############# end netbsd packages #####

############# freebsd packages #####
cd /mnt/personal/systems/bsd-based/freebsd && ./freebsd
cd $PROJECT_DIR
############# end freebsd packages #####

#slackware packages must be last because we are asked for root
#password
############# slackware packages #########
echo
echo "Creating slackware packages..."
echo
sleep 2;

USER_ID=`id -u`;
USER_GROUP=`id -g`;
SLACK_TEMP=/tmp/slack_temp;

export PATH=$PATH:/sbin && \
dchroot -d -c slackware "\
su -c '\
CFLAGS=\"-O2 -march=i486 -mcpu=i486\";\
LDFLAGS=\"\";\
rm -rf $SLACK_TEMP;\
mkdir -p $SLACK_TEMP/libmp3splt/usr/doc/libmp3splt;\
mkdir -p $SLACK_TEMP/libmp3splt/install;\
cd libmp3splt; ./autogen.sh && ./configure --prefix=/usr && \
make clean && make&&\
make DESTDIR=$SLACK_TEMP/libmp3splt install || exit 1;\
cp $LIBMP3SPLT_DOC_FILES $SLACK_TEMP/libmp3splt/usr/doc/libmp3splt;\
cp slackware/slack-* $SLACK_TEMP/libmp3splt/install;\
make install;ldconfig;\
cd $SLACK_TEMP/libmp3splt;\
makepkg -l y -c y libmp3splt-$LIBMP3SPLT_VERSION-i486.tgz;\
mkdir -p $SLACK_TEMP/mp3splt-gtk/usr/doc/mp3splt-gtk;\
mkdir -p $SLACK_TEMP/mp3splt-gtk/install;\
cd $PROJECT_DIR/mp3splt-gtk; ./autogen.sh && \
./configure --enable-bmp --prefix=/usr &&\
make clean && make&&\
make DESTDIR=$SLACK_TEMP/mp3splt-gtk install || exit1;\
cp $MP3SPLT_GTK_DOC_FILES $SLACK_TEMP/mp3splt-gtk/usr/doc/mp3splt-gtk;\
cp slackware/slack-* $SLACK_TEMP/mp3splt-gtk/install;\
cd $SLACK_TEMP/mp3splt-gtk;\
makepkg -c y mp3splt-gtk-$MP3SPLT_GTK_VERSION-i486.tgz;\
mkdir -p $SLACK_TEMP/mp3splt/usr/doc/mp3splt;\
mkdir -p $SLACK_TEMP/mp3splt/install;\
cd $PROJECT_DIR/newmp3splt; ./autogen.sh && \
./configure --prefix=/usr && \
make clean && make&&\
make DESTDIR=$SLACK_TEMP/mp3splt install || exit 1;\
cp $MP3SPLT_DOC_FILES $SLACK_TEMP/mp3splt/usr/doc/mp3splt;\
cp slackware/slack-* $SLACK_TEMP/mp3splt/install;\
cd $SLACK_TEMP/mp3splt;\
makepkg -c y mp3splt-$MP3SPLT_VERSION-i486.tgz;\
cd $PROJECT_DIR/libmp3splt && make uninstall && make clean;\
cd $PROJECT_DIR/mp3splt-gtk && make clean;\
cd $PROJECT_DIR/newmp3splt && make clean;\
mv $SLACK_TEMP/mp3splt/mp3splt*.tgz $PROJECT_DIR;\
mv $SLACK_TEMP/libmp3splt/libmp3splt*.tgz $PROJECT_DIR;\
mv $SLACK_TEMP/mp3splt-gtk/mp3splt-gtk*.tgz $PROJECT_DIR;\
chown $USER_ID:$USER_GROUP $PROJECT_DIR*.tgz;\
rm -rf $SLACK_TEMP;'" || exit 1
############# end slackware packages #####

############# nexenta gnu/opensolaris packages #####
cd /mnt/personal/systems/opensolaris/ && ./nexenta
cd $PROJECT_DIR
############# end nexenta gnu/opensolaris packages #####

############# finish packaging #####
echo
echo "Finishing packaging..."
echo
sleep 2;

#copy packages to the new directory
#the new release directory
RELEASE_DIR=release_$LIBMP3SPLT_VERSION;

mkdir -p $RELEASE_DIR
rm -rf $RELEASE_DIR/*

#debian
mv ./*sarge*.deb ./$RELEASE_DIR || exit 1
mv ./*etch*.deb ./$RELEASE_DIR || exit 1
mv ./*sid*.deb ./$RELEASE_DIR || exit 1
#ubuntu
mv ./*breezy*.deb ./$RELEASE_DIR || exit 1
mv ./*dapper*.deb ./$RELEASE_DIR || exit 1
mv ./*edgy*.deb ./$RELEASE_DIR || exit 1
#nexenta
mv ./*solaris*.deb ./$RELEASE_DIR || exit 1
#windows
mv ./*.exe ./$RELEASE_DIR || exit 1
#openbsd
mv ./*obsd*.tgz ./$RELEASE_DIR || exit 1
#netbsd
mv ./*nbsd*.tgz ./$RELEASE_DIR || exit 1
#freebsd
mv ./*fbsd*.tbz ./$RELEASE_DIR || exit 1
#gnu/linux static+dynamic
mv ./*_static_$ARCH.tar.gz ./$RELEASE_DIR || exit 1
mv ./*_dynamic_$ARCH.tar.gz ./$RELEASE_DIR || exit 1
#arch linux
mv ./*pkg.tar.gz ./$RELEASE_DIR || exit 1
#gentoo ebuilds
mv ./*ebuild*.tar.gz ./$RELEASE_DIR || exit 1
#source code
mv ./*.tar.gz ./$RELEASE_DIR || exit 1
#rpms
mv ./*.rpm ./$RELEASE_DIR || exit 1
#slackware
mv ./*.tgz ./$RELEASE_DIR || exit 1
############# end finish packaging #####

############# uploading to sourceforge.net #####
#we put the files on the sourceforge ftp
if [[ $UPLOAD_TO_SOURCEFORGE == 1 ]]; then
    echo
    echo "Uploading files to sourceforge.net..."
    echo
    sleep 2;
    for a in ls $DIST_VERSION; do
        lftp -e "cd /incoming;put $DIST_VERSION/$a;quit" -u anonymous,\
upload.sourceforge.net || exit 1
    done
fi
############# finish uploading to sourceforge.net #####

echo
echo "The packaging is finished."
echo
