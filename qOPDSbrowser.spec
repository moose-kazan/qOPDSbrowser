Name: qopdsbrowser
Version: %PKG_VERSION
Release: %PKG_RELEASE
Summary: Simple OPDS browser. Written with C++/Qt.

Group: Office
License: LGPL-2.1
URL: https://github.com/moose-kazan/qopdsbrowser
BuildRequires: qt6-qtbase-devel, qt6-qttools-devel, cmake
Source0: https://github.com/moose-kazan/qopdsbrowser/archive/refs/tags/v%version.zip

%description
Simple OPDS browser. Written with C++/Qt.
It allow browse OPDS feeds like web/ftp sites and download books.

%build
%cmake
%cmake_build

%install
%cmake_install

%files
/usr/bin/qOPDSbrowser
/usr/share/applications/qOPDSbrowser.desktop
/usr/share/icons/hicolor/128x128/apps/qOPDSbrowser.png
/usr/share/icons/hicolor/16x16/apps/qOPDSbrowser.png
/usr/share/icons/hicolor/24x24/apps/qOPDSbrowser.png
/usr/share/icons/hicolor/256x256/apps/qOPDSbrowser.png
/usr/share/icons/hicolor/32x32/apps/qOPDSbrowser.png
/usr/share/icons/hicolor/48x48/apps/qOPDSbrowser.png
/usr/share/icons/hicolor/512x512/apps/qOPDSbrowser.png
/usr/share/icons/hicolor/64x64/apps/qOPDSbrowser.png
/usr/share/icons/hicolor/96x96/apps/qOPDSbrowser.png
/usr/share/icons/qOPDSbrowser.png

%changelog
