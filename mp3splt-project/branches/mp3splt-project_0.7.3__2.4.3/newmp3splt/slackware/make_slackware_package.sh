#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh || exit 1

put_package "slackware"

#if we don't have the distribution file
DIST_FILE="../mp3splt-${MP3SPLT_VERSION}-${ARCH}.tgz"
if [[ ! -f $DIST_FILE ]];then
    SLACK_TEMP=/tmp/slack_temp
    
    #we set the necessary flags
    #export CFLAGS="-O2 -march=$ARCH -mcpu=$ARCH"
    export CFLAGS="$CFLAGS"
    export LDFLAGS="$LDFLAGS"

    #we create the needed directories
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e $SLACK_TEMP ]];then
        mv $SLACK_TEMP $SLACK_TEMP/mp3splt${DATEMV}
    fi
    mkdir -p $SLACK_TEMP/mp3splt/usr/doc/mp3splt
    mkdir -p $SLACK_TEMP/mp3splt/install

    #we compile
    ./autogen.sh &&\
        ./configure --prefix=/usr &&\
        make clean &&\
        make &&\
        make DESTDIR=$SLACK_TEMP/mp3splt install &&\
        cp $MP3SPLT_DOC_FILES $SLACK_TEMP/mp3splt/usr/doc/mp3splt &&\
        cp slackware/slack-* $SLACK_TEMP/mp3splt/install &&\
        cd $SLACK_TEMP/mp3splt &&\
        /sbin/makepkg -l y -c y mp3splt-${MP3SPLT_VERSION}-$ARCH.tgz &&\
        mv mp3splt-${MP3SPLT_VERSION}-$ARCH.tgz $PROGRAM_DIR/.. || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
