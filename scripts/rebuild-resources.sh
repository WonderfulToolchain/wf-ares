#!/bin/sh

# wf-ares: Ares's upstream contains binary blobs in two locations: as files
# themselves, and in pre-compiled resource.cpp files.
# As the "deblobbing" scripts remove both, it is necessary to re-generate the
# resource.cpp files at build time.
cd "$(dirname "$0")"/.. || exit 1

cd sourcery
make || exit 1
PATH=`pwd`/out:"$PATH"
cd ..

cd hiro/resource
make || exit 1
cd ../..

cd mia/resource
make || exit 1
cd ../..

cd ares/ares/resource
make || exit 1
cd ../../..

cd desktop-ui/resource
make || exit 1
cd ../../..
