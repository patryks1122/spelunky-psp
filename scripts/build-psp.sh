#!/usr/bin/env bash
# This script is meant to be run from the root of the project.

source scripts/variables-psp.sh

cd $BUILD_PATH
cmake --build . --target install --config Release -- -j `nproc`
