#!/bin/bash

component_requested() {
	needle=$1
	shift
	while (( "$#" )); do
		if [ "$1" = "all" ] || [ "$1" = "$needle" ]; then
			return 0
		fi
		shift
	done
	return 1
}

BUILD_PREFIX=build

if component_requested gb $@; then
	echo "=== Building SameBoot (GB/GBC) ==="
	pushd SameBoy
	make bootroms
	popd
fi
