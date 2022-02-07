#!/usr/bin/env bash
PACKAGE=tiki
VERSION=3.4
SRCDIR=$(pwd)
BUILDDIR=/tmp/build-${PACKAGE}-${VERSION}

DEBEMAIL="dimitrisinbox@gmail.com"
DEBFULLNAME="Dimitris Psathas"
export DEBEMAIL DEBFULLNAME

mkdir -p "${BUILDDIR}" && pushd "${BUILDDIR}"
qmake PREFIX=/usr "${SRCDIR}/tiki.pro" CONFIG+=release
make
INSTALL_ROOT=${PACKAGE}-${VERSION} make install
mkdir -p ${PACKAGE}-${VERSION}/DEBIAN
cat > ${PACKAGE}-${VERSION}/DEBIAN/control <<EOF
Package: ${PACKAGE}
Version: ${VERSION}
Section: base
Priority: optional
Architecture: amd64
Depends: libqt5gui5, libqt5printsupport5
Maintainer: ${DEBFULLNAME} <${DEBEMAIL}>
Description: Tiki
 A simple project organizer
EOF

dpkg-deb --build ${PACKAGE}-${VERSION}
cp "${PACKAGE}-${VERSION}.deb" "${SRCDIR}"
popd
rm -rf "${BUILDDIR}"
make clean
rm -rf "${PACKAGE}-${VERSION}"
sudo alien -r *.deb
echo
echo "Packages created."
echo
