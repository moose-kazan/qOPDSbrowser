#!/bin/bash

ROOTPATH=$(dirname $(dirname $(readlink -f $0)))

IMGNAME=qopdsbrowser-noble-build

PKG_VER="$1"
PPA_VER="$2"


if [ -z $PPA_VER ]; then
    echo "Usage:"
    echo "  $0 VERSION BUILD"
    echo " "
    echo "Example:"
    echo "  $0 0.0.6 1"
    exit
fi


docker build -f package/ubuntu/Dockerfile -t $IMGNAME $ROOTPATH

docker run --rm -it \
    -e PKG_VERSION=${PKG_VER} \
    -e PKG_PPAVER=${PPA_VER} \
    -v $ROOTPATH:/source \
    -t $IMGNAME
