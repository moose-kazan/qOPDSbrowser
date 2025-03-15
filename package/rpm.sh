#!/bin/bash

ROOTPATH=$(dirname $(dirname $(readlink -f $0)))


PKG_VER="$1"
PPA_VER="$2"
DST_DISTR="$3"

DISTR_LIST="redos80 fedora41"

test -z "${DST_DISTR}" || DISTR_LIST="${DST_DISTR}"

buildpkg() {
	PKG_VER="$1"
	PPA_VER="$2"
	PKG_DISTR="$3"

	IMGNAME=qopdsbrowser-${PKG_DISTR}-build

	if [ -z $PPA_VER ]; then
	    echo "Usage:"
	    echo "  $0 VERSION BUILD"
	    echo " "
	    echo "Example:"
	    echo "  $0 0.0.6 1"
	    exit
	fi

        if [ -z $PKG_DISTR ]; then
	    echo "Distr name can't be empty!"
	    exit
	fi

	docker build -f package/rpm/Dockerfile.${PKG_DISTR} -t $IMGNAME $ROOTPATH

	docker run --rm -it \
	    -e PKG_VERSION=${PKG_VER} \
	    -e PKG_RELEASE=${PPA_VER} \
	    -v $ROOTPATH:/source \
	    -t $IMGNAME
}

for distr in $DISTR_LIST; do
	buildpkg $PKG_VER $PPA_VER $distr
done
