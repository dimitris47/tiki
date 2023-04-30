#!/usr/bin/env bash

IFS=" = " read _ ver < tiki.pro
VERSION=$ver
PACKAGE=tiki
SRCDIR=$(pwd)
BUILDDIR=/tmp/build-${PACKAGE}-${VERSION}

DEBEMAIL="dimitrisinbox@gmail.com"
DEBFULLNAME="Dimitris Psathas"
export DEBEMAIL DEBFULLNAME

mkdir -p "${BUILDDIR}" && pushd "${BUILDDIR}"
~/Qt/5.15.2/gcc_64/bin/qmake PREFIX=/usr "${SRCDIR}/tiki.pro" CONFIG+=release
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
Description: Simple project-tasks manager
EOF
dpkg-deb --build ${PACKAGE}-${VERSION}
cp "${PACKAGE}-${VERSION}.deb" "${SRCDIR}"
popd
rm -rf "${BUILDDIR}"
make clean
rm -rf "${PACKAGE}-${VERSION}"
echo

rm ../*.deb
mv *.deb ..
cd ..
sudo dpkg -i *.deb
