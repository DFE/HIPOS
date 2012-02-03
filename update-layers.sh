#!/bin/bash

set -e

update_layer()
{
	echo "Updating layer: $1"
	pushd $1
	git checkout master
	git pull
	popd
	git add $1
}

update_layer bitbake
update_layer meta-angstrom
update_layer meta-openembedded
update_layer meta-texasinstruments
update_layer openembedded-core
update_layer meta-java
