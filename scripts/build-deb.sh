#/bin/bash

# $1 - deb-package version

set -e

cd ../package

dpkg-deb --build --root-owner-group . ../releases/packages/"kernel-de-${1}.deb"

exit 0
