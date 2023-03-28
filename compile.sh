#!/bin/sh
set -e

cmake -B build -G Ninja \
	-DCMAKE_BUILD_TYPE=RelWithDebInfo

ninja -C build
