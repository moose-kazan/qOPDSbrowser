#!/bin/bash

cd /
cp -a /source /src
cd /src
rm -rf build
git checkout v${PKG_VERSION}

mkdir -p build
cmake -S. -Bbuild
cmake --build build
RET=$?

if [ "$RET" != "0" ]; then
    echo "Can't build binary"
    exit
fi

cd /
rm -rf /src
cp -a /source /src
cd /src
rmm -rf build
git checkout v${PKG_VERSION}

PKG_DISTR=$(lsb_release -cs)

echo "qopdsbrowser (${PKG_VERSION}-${PKG_DISTR}${PKG_PPAVER}) ${PKG_DISTR}; urgency=medium

  * New upstream release

 -- Vadim Kalinnikov <moose@ylsoftware.com>  $(date --rfc-email)" > debian/changelog

dpkg-buildpackage --root-command=fakeroot --build=source

mkdir -p /source/release

mv -v ../qopdsbrowser_${PKG_VERSION}* /source/release/
