#!/bin/bash
#
# $Id: hidav-init.sh 86429 2011-12-08 07:43:35Z nilius $
#


#############################
#                           #
#  D A N G E R    Z O N E   #
#                           #
#############################
#
# This must _always_ be set to the name of this script 
#  or "sourceing" detection will fail. 

script_name="hidav-init.sh"


#############################

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
  BB_LAYER_CONF_TEMPLATE="build/conf/bblayers.conf.template"
  BB_LAYER_CONF="build/conf/bblayers.conf"

  log "I: generating ${BB_LAYER_CONF}"

  "${GIT}" checkout ${BB_LAYER_CONF_TEMPLATE}

  cp ${BB_LAYER_CONF_TEMPLATE} ${BB_LAYER_CONF}

  echo "BBLAYERS = \" \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-hidav \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-hidav-ti81xx \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-intel \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-intel/meta-cedartrail \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-hidav-intel \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-java \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-texasinstruments \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-angstrom \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-oe \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-systemd \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-efl \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-gnome \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/openembedded-core/meta \\" >> ${BB_LAYER_CONF}

  echo "\"" >> ${BB_LAYER_CONF}
}

update_local_conf()
{
  # update local conf with local parallism settings
  LOCAL_CONF="build/conf/local.conf"
  PARALLEL_CONF="build/conf/parallel.conf"

  if [ -f ${PARALLEL_CONF} ]; then
    log "I: updating ${LOCAL_CONF}"

    SED_THREADS=`sed -n '/^[[:space:]]*BB_NUMBER_THREADS[?:[:space:]]*=.*/p' ${PARALLEL_CONF}`
    SED_PAR_MAKE=`sed -n '/^[[:space:]]*PARALLEL_MAKE[?:[:space:]]*=.*/p' ${PARALLEL_CONF}`

    sed -i "s/^[[:space:]]*BB_NUMBER_THREADS[?:[:space:]]*=.*/$SED_THREADS/" ${LOCAL_CONF}
    sed -i "s/^[[:space:]]*PARALLEL_MAKE[?:[:space:]]*=.*/$SED_PAR_MAKE/" ${LOCAL_CONF}
  else
    log "I: ${PARALLEL_CONF} not found, BB_NUMBER_THREADS and PARALLEL_MAKE will use values from ${LOCAL_CONF}"
  fi
}

update_submodules()
{
  log "I: updating submodules (OE layers + bitbake)"

   "${GIT}" submodule init
   "${GIT}" submodule sync
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
  update_local_conf

  return 0
}

echo -e "***"
echo -e '***    $Id: hidav-init.sh 86429 2011-12-08 07:43:35Z nilius $'
echo -e "***"

if [ "`basename $0`" != "$script_name" ]; then
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
