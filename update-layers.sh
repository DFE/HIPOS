#!/bin/bash

set -e

this="$(dirname $0)"

update_layer()
{
	echo "Updating layer: $1"
	pushd $this/$1
	git checkout master
	git pull
	popd
}

update_layer bitbake
update_layer meta-angstrom
update_layer meta-openembedded
update_layer meta-texasinstruments
update_layer openembedded-core
update_layer meta-java
update_layer night-owl
update_layer meta-hidav-ti81xx
update_layer meta-hidav-intel
update_layer meta-intel
