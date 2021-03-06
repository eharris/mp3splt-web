#!/bin/bash

#this script is included by other scripts 
#and only contains variables used in other scripts

################# variables to set ############

#build architecture
ARCH=${ARCH:=`uname -m`}
#mp3splt version
MP3SPLT_VERSION=2.4
#the libmp3splt version (if mp3splt is only compatible with this
#libmp3splt version)
LIBMP3SPLT_VERSION=0.7
#the mp3splt documentation files
MP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)
#the mp3splt installed files (needed for openbsd, freebsd, ...)
MP3SPLT_FILES=(bin/mp3splt)
#mp3splt man page
MP3SPLT_MAN1_FILES=(mp3splt.1)

#description of the package
#used for rpm, freebsd and netbsd
#slackware has separate description file
MP3SPLT_DESCRIPTION="mp3splt-project common features:
* split mp3 and ogg files from a begin time to an end time without decoding
* split an album with splitpoints from the freedb.org server
* split an album with local .XMCD, .CDDB or .CUE file
* split files automatically with silence detection
* split files by a fixed time length
* split files created with Mp3Wrap or AlbumWrap
* split concatenated mp3 files
* support for mp3 VBR (variable bit rate)
* specify output directory for split files"

#we put i386 if i686 except for archlinux, where we set i686 inside
#its make_ script
if [[ $ARCH = "i486" || $ARCH = "i586" || $ARCH = "i686" || $ARCH = "i86pc" ]];then
    ARCH="i386";
fi

#compilation flags
if [[ $1 != "noflags" && $1 != "quiet_noflags" ]];then
    if [[ $ARCH = "i386" ]];then
        CFLAGS="-O2 -march=${ARCH}"
        LDFLAGS=""
    fi
fi

################# end variables to set ############

#print cyan color
function p_cyan
{
    echo -e "\033[36m${@}\033[0m"
}
#print blue color
function p_blue
{
    echo -e "\033[1;34m${@}\033[0m"
}
#print yellow color
function p_yellow
{
    echo -e "\033[33m${@}\033[0m"
}
#we put the package name
function put_package
{
    echo -n $'Package\t\t: '
    p_blue "$1"
    echo
}
#we put the warning
function put_is_package_warning
{
    p_cyan "$1"
}

if [[ $1 != "quiet" && $1 != "quiet_noflags" ]];then
    echo
    echo -n $'Application\t: '
    p_yellow "mp3splt"
    echo $'Architecture\t: '$ARCH
    echo $'Version\t\t: '$MP3SPLT_VERSION
fi
