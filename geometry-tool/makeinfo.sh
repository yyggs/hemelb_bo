#!/bin/bash
# This file is part of HemeLB and is Copyright (C)
# the HemeLB team and/or their institutions, as detailed in the
# file AUTHORS. This software is provided under the terms of the
# license in the file LICENSE.

# When installed by conda or the default CMake, VTK, ITK, and VMTK do
# not show up to setuptools as they lack dist-info directories. Create
# these.

# This script was modified to handle case where jq is not found

SITE_PACKAGES_DIR=$(python -c 'import sysconfig; print(sysconfig.get_path("platlib"))')

function make_dist_info() {
NAME=$1

# if pip list --format json | python -c "import sys, json; data = json.load(sys.stdin); packages = [pkg['name'] for pkg in data]; print('$NAME' in packages)" > /dev/null; then
# echo "pip already knows about $NAME"
# return 0
# fi

VERSION=$(conda list --json $NAME | python -c "import sys, json; data = json.load(sys.stdin); print(data[0]['version'] if data else '')")
if [ -z "$VERSION" ]; then
echo "Couldn't get version for $NAME"
exit 1
fi

if [ ! -d $SITE_PACKAGES_DIR/$NAME ]; then
echo "No package dir '$SITE_PACKAGES_DIR/$NAME'"
exit 1
fi

distinfo=$SITE_PACKAGES_DIR/$NAME-$VERSION.dist-info
mkdir -p $distinfo
cat > $distinfo/METADATA <<EOF
Metadata-Version: 2.1
Name: $NAME
Version: $VERSION
EOF
}

# make_dist_info vtk
make_dist_info vmtk