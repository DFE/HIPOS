#!/bin/bash
# vim:set ts=4 sw=4 noexpandtab:
#
# \brief Build specific init script example.
# \description This script has to be placed into the root repository and should be
#              called after fetching the root repository.
#              It initializes/updates all submodules and creates appropriate 
#              bitbake configurations. Optionally, local sublayer can be included
#              here.
# 
# \author Ralf Schröder
# \pre GIT (environment) optionally set to the git binary
#         

#############################
#                           #
#  D A N G E R    Z O N E   #
#                           #
#############################
#
# This must _always_ be set to the name of this script 
#  or "sourceing" detection will fail. 

INIT_SCRIPT_NAME="hipos-init.sh"


#############################

# Log helper to realize project specific log locations 
log()
{
	local TIMESTAMP=`date -u +%Y-%m-%d-%H:%M:%S`

	# to the logfile
	echo "${TIMESTAMP} [HIPOS Build] $@"

	# and to the new stdout
	[ -f "${BUILD_LOG}" ] && echo "${TIMESTAMP} [HIPOS Build] $@" >&3

	# clear return value polluted by the test above (reason for a lot of headaches...)
	return 0
}

# Project specific sanity checks.
# We check for git and set GIT as variable
sanity()
{
	log "I: linux sanity check"

	if  test -z "$GIT" || ! $GIT --version > /dev/null; then
		export GIT=`which git`
	fi
	if  test -z "$GIT" || ! $GIT --version > /dev/null; then
		log "E: couldn't find git" && return 1;
	fi
	log "I: check `$GIT --version`: OK"

	return 0
}


# Helper to check existence of a given file, tries to git-checkout if not exists.
check_file()
{
	if [ ! -f "$1" ]; then
		log "I: $1 not found, checking out from git"
		"${GIT}" checkout $1 &&
		test -f "$1" ||
		{ log "E: $1 not available from git repository"; return 1; }
	fi

	return 0
}

main_init()
{
	echo -e '***'
	echo -e '***    '$INIT_SCRIPT_NAME
	echo -e '***'

	sanity &&

	# fetch the base init script an start it
	if [ ! -f hipos-base/init.sh ]; then
		"${GIT}" submodule init hipos-base &&
		"${GIT}" submodule sync hipos-base &&
		"${GIT}" submodule update hipos-base ||
		{ log "E: updating hipos-base failed"; return 1; }
	fi &&
	check_file hipos-base/init.sh &&
	. hipos-base/init.sh &&

	# optional: modify layer and update variables here...
    #           BB_LAYERS_INCLUDED: ordered list of default layers
    #           BB_BUILD_DIR_BASE: temporary build directory

	{ test "`basename -- \"$0\"`" != "$INIT_SCRIPT_NAME" && SCRIPT_SOURCED=true || SCRIPT_SOURCED=false; } &&
	hipos_base_init ||
	{ log "E: bitbake initialization failed"; return 1; }
}

main_init

