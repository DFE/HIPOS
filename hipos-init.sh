!/bin/bash

#############################
#                           #
#  D A N G E R    Z O N E   #
#                           #
#############################
#
# This must _always_ be set to the name of this script 
#  or "sourceing" detection will fail. 

script_name="hipos-init.sh"


#############################

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
  # generate layer conf from template
  
  BB_LAYER_CONF_TEMPLATE="build/conf/bblayers.conf.template"
  BB_LAYER_CONF="build/conf/bblayers.conf"

  log "I: generating ${BB_LAYER_CONF}"

  if [ ! -f ${BB_LAYER_CONF_TEMPLATE} ]; then
    log "I: ${BB_LAYER_CONF_TEMPLATE} not found, checking out from git"
    "${GIT}" checkout ${BB_LAYER_CONF_TEMPLATE}
  fi

  cp ${BB_LAYER_CONF_TEMPLATE} ${BB_LAYER_CONF}
  sed -i "1i\# WARNING:\n# This file is automatically generated by hipos-init.sh\n# Changes should be made in ${BB_LAYER_CONF_TEMPLATE}\n#\n#" ${BB_LAYER_CONF}

  echo "BBLAYERS = \" \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-hipos \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-hipos-kirkwood \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-java \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-angstrom \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-oe \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-systemd \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-multimedia \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-networking \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-efl \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/meta-openembedded/meta-gnome \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/openembedded-core/meta \\" >> ${BB_LAYER_CONF}

  echo "\"" >> ${BB_LAYER_CONF}
}

update_local_conf()
{
  # generate local.conf from template
  # optional: apply local settings made in private.conf
  # (local.conf is tracked via git and thus not truly local)
  
  LOCAL_CONF="build/conf/local.conf"
  LOCAL_CONF_TEMPLATE="build/conf/local.conf.template"
  PRIVATE_CONF="build/conf/private.conf"

  log "I: generating ${LOCAL_CONF}"

  if [ ! -f ${LOCAL_CONF_TEMPLATE} ]; then
    log "I: ${LOCAL_CONF_TEMPLATE} not found, checking out from git"
    "${GIT}" checkout ${LOCAL_CONF_TEMPLATE}
  fi
    
  if [ -f ${PRIVATE_CONF} ]; then
    awk -f do_private_conf.awk ${PRIVATE_CONF} ${LOCAL_CONF_TEMPLATE} > ${LOCAL_CONF}
  else
    log "I: ${PRIVATE_CONF} not found, only ${LOCAL_CONF_TEMPLATE} is used"
    cp ${LOCAL_CONF_TEMPLATE} ${LOCAL_CONF}
  fi

  sed -i "1i\# WARNING:\n# This file is automatically generated by hipos-init.sh\n# Changes should be made in ${LOCAL_CONF_TEMPLATE} and/or ${PRIVATE_CONF}\n#\n#" ${LOCAL_CONF}
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
echo -e '***    $Id: hipos-init.sh $'
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