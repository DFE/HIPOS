#!/bin/sh
set -x

ME=$(dirname $0)
cd $ME/testsuite00001
virtualenv .
set +x
echo "! activate virtualenv"
. bin/activate
set -x
pip install -r requirements.txt
nosetests

set +x
echo "! deactivate virtualenv"
deactivate
set -x

cd -
