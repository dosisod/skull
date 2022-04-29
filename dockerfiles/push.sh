#!/bin/bash

set -e

version="${1:1}"
registry="ghcr.io/dosisod/skull"

[ -z "$version" ] && {
	echo "usage: $0 vX.Y.Z"
	exit 1
}

for base in {alpine,archlinux,ubuntu}
do
	docker tag skull:$base "$registry:$version-$base"
	docker tag skull:$base "$registry:$base"
done

docker tag skull:latest "$registry:$version-ubuntu"

docker push -a $registry
