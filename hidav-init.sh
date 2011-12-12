#!/bin/bash
#
# $Id: hidav-init.sh 86429 2011-12-08 07:43:35Z nilius $
#

log()
{
  local TIMESTAMP=`date -u +%Y-%m-%d-%H:%M:%S`

  # to the logfile
  echo "${TIMESTAMP} [HiDav Build] $@"

  # and to the new stdout
  [ -f "${BUILD_LOG}" ] && echo "${TIMESTAMP} [HiDav Build] $@" >&3

  # clear return value polluted by the test above (reason for a lot of headaches...)
  return 0
}

sanity()
{
  log "I: linux sanity check"

  GIT=`which git`
  [ -z "${GIT}" ] && log "E: couldn't find git" && return 1;
  log "I: check git: OK"

  export GIT

  return 0
}

update_bblayers_conf()
{
  # update layer conf
  BB_LAYER_CONF="build/conf/bblayers.conf"

  log "I: updating ${BB_LAYER_CONF}"

  "${GIT}" checkout ${BB_LAYER_CONF}

  echo "BBLAYERS = \" \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-hidav \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-texasinstruments \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-angstrom \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-oe \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/openembedded-core/meta \\" >> ${BB_LAYER_CONF}

  echo "\"" >> ${BB_LAYER_CONF}
}

update_submodules()
{
  log "I: updating submodules (OE layers + bitbake)"

   "${GIT}" submodule init
   "${GIT}" submodule update
}

init()
{
  BB_LINK="openembedded-core/bitbake"

  update_submodules

  if [ ! -h "${BB_LINK}" ]; then
    ln -s "../bitbake" "${BB_LINK}"
    if [ "$?" -ne "0" ]; then
      log "E: cannot set link to bitbake"
      return 1
    fi
  fi

  update_bblayers_conf

  return 0
}

echo -e "***"
echo -e '***    $Id: hidav-init.sh 86429 2011-12-08 07:43:35Z nilius $'
echo -e "***"

if [ "`basename $0`" = "bash" ]; then
  SOURCED=1
fi

BASE="`pwd`"

sanity
if [ "$?" -ne "0" ]; then
log "E: sanity check failed"
  exit 1
fi

init
if [ "$?" -ne "0" ]; then
  log "E: init failed"
  exit 1
fi

INIT_SCRIPT="init.sh"

echo "source ${BASE}/openembedded-core/oe-init-build-env ${BASE}/build" > "${INIT_SCRIPT}"
chmod +x ${INIT_SCRIPT}

if [ "${SOURCED}" = "1" ]; then
  . ${INIT_SCRIPT}

  echo -e "***"
  echo -e "***"
  echo -e "***    You can now start building:"
  echo -e "***"
  echo -e "***    $ bitbake \e[00;36m<target>\e[00m"
  echo -e "***"
else
  echo -e "***"
  echo -e "***"
  echo -e "***    You have to run the following prior build:"
  echo -e "***"
  echo -e "***    $ \e[00;36msource ${BASE}/${INIT_SCRIPT}\e[00m"
  echo -e "***"
  echo -e "***"

  exit 0
fi
