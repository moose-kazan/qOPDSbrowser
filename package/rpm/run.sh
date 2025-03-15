#!/bin/bash

PKG_DISTR=$1
test -z "${PKG_DISTR}" || PKG_RELEASE="${PKG_RELEASE}.${PKG_DISTR}"

cd /
cp -a /source /src
cd /src
rm -rf build
git checkout v${PKG_VERSION}

echo " * $(LANG=C date "+%a %b %d %Y") Vadim Kalinnikov <moose@ylsoftware.com> - ${PKG_VERSION}-${PKG_RELEASE}
 - New upstream release
" >> qOPDSbrowser.spec


rpmbuild --build-in-place \
        -D "PKG_VERSION ${PKG_VERSION}" \
        -D "PKG_RELEASE ${PKG_RELEASE}" \
        -bb qOPDSbrowser.spec

mv -v /root/rpmbuild/RPMS/x86_64/qopdsbrowser-${PKG_VERSION}* /source/release/
