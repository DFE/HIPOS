#!/bin/bash
#
# $Id: hidav-init.sh 86429 2011-12-08 07:43:35Z nilius $
#

OE_CORE_NAME=oe-core
OE_CORE_URI=git://git.openembedded.org/openembedded-core
OE_CORE_BRANCH=master
OE_CORE_COMMIT_ID=

META_OE_NAME=meta-openembedded
META_OE_URI=http://cgit.openembedded.org/cgit.cgi/meta-openembedded
META_OE_BRANCH=master
META_OE_COMMIT_ID=

META_TI_NAME=meta-texasinstruments
META_TI_URI=http://git.angstrom-distribution.org/cgi-bin/cgit.cgi/meta-texasinstruments
META_TI_BRANCH=master
META_TI_COMMIT_ID=

META_ANGSTROM_NAME=meta-angstrom
META_ANGSTROM_URI=http://git.angstrom-distribution.org/cgi-bin/cgit.cgi/meta-angstrom
META_ANGSTROM_BRANCH=master
META_ANGSTROM_COMMIT_ID=

BITBAKE_NAME=bitbake
BITBAKE_URI=git://git.openembedded.org/bitbake
BITBAKE_BRANCH=master
BITBAKE_COMMIT_ID=

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

  SVN=`which svn`
  [ -z "${SVN}" ] && log "E: couldn't find svn" && return 1;
  log "I:  check svn: OK"

  GIT=`which git`
  [ -z "${GIT}" ]  && log "E: couldn't find git" && return 1;
  log "I:  check git: OK"

  export SVN GIT

  return 0
}

get_from_git()
{
  local name=$1
  local uri=$2
  local branch=$3
  local commit_id=$4

  log "I: get ${name}"

  local target_dir="${WORKDIR}/${name}"

  if [ -d "${target_dir}" -a -d "${target_dir}/.git" ]; then
    log "I:  updating ${name}"

    pushd "${target_dir}" > /dev/null
    # cleanup local tree state
    "${GIT}" reset --hard HEAD > /dev/null
    # switch to master
    "${GIT}" checkout -q master
    "${GIT}" pull
    # checkout clean ${branch}
    if [ -n "${branch}" ]; then
       log "I:  checking out branch ${branch}"
      "${GIT}" checkout ${branch}
    fi

    # checkout specific commit if requested
    if [ -n "${commit_id}" ]; then
      log "I:  checking out commit ${commit_id}"
      "${GIT}" branch -D work
      "${GIT}" checkout -b work "${commit_id}"
    fi
    popd > /dev/null
  else
    log "I:  checking out from ${uri}"

    rm -rf "${target_dir}"
    "${GIT}" clone ${uri} "${target_dir}"
    pushd "${target_dir}" > /dev/null

     # checkout ${branch}
    if [ -n "${branch}" ]; then
      log "I:  checking out branch ${branch}"
      "${GIT}" checkout ${branch}
    fi

    # checkout specific commit if requested
    if [ -n "${commit_id}" ]; then
      log "I:  checking out commit ${commit_id}"
      "${GIT}" checkout -b work "${commit_id}"
    fi
    popd > /dev/null
  fi
}

update_bblayers_conf()
{
  # update layer conf
  BB_LAYER_CONF="${WORKDIR}/${OE_CONF_NAME}/conf/bblayers.conf"

  log "I: updating ${BB_LAYER_CONF}"

  echo "BBLAYERS = \" \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/${OE_HIDAV_LAYER_NAME} \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/${META_TI_NAME} \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/${META_ANGSTROM_NAME} \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/${META_OE_NAME}/meta-oe \\" >> ${BB_LAYER_CONF}
  echo "${BASE}/${OE_CORE_NAME}/meta \\" >> ${BB_LAYER_CONF}

  echo "\"" >> ${BB_LAYER_CONF}
}

init()
{
  get_from_git ${OE_CORE_NAME} ${OE_CORE_URI} ${OE_CORE_BRANCH} ${OE_CORE_COMMIT_ID}
  get_from_git ${META_OE_NAME} ${META_OE_URI} ${META_OE_BRANCH} ${META_OE_COMMIT_ID}
  get_from_git ${META_TI_NAME} ${META_TI_URI} ${META_TI_BRANCH} ${META_TI_COMMIT_ID}
  get_from_git ${META_ANGSTROM_NAME} ${META_ANGSTROM_URI} ${META_ANGSTROM_BRANCH} ${META_ANGSTROM_COMMIT_ID}
  get_from_git ${BITBAKE_NAME} ${BITBAKE_URI} ${BITBAKE_BRANCH} ${BITBAKE_COMMIT_ID}

  BB_LINK="${WORKDIR}/${OE_CORE_NAME}/bitbake"

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

WORKDIR=oe_work

echo -e "***"
echo -e '***    $Id: hidav-init.sh 86429 2011-12-08 07:43:35Z nilius $'
echo -e "***"

if [ "`basename $0`" = "bash" ]; then
  SOURCED=1
fi

if [ -n "$1" ]; then
  WORKDIR="$1"
else
  echo -e "***    Will use \e[01;32m$WORKDIR\e[00m as build dir. To specifiy a custom build dir use:"
  echo -e "***"
  echo -e "***    $ hidav-init.sh \e[00;36m<build-dir>\e[00m"
  echo -e "***"
fi

BASE="`pwd`/${WORKDIR}"

log "I: workdir: ${WORKDIR}"

mkdir -p "${WORKDIR}"
if [ "$?" -ne "0" ]; then
  log "E: cannot mkdir ${WORKDIR}"
  exit 1
fi

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

echo "source ${BASE}/${OE_CORE_NAME}/oe-init-build-env ${BASE}/${OE_CONF_NAME}" > "${WORKDIR}/${INIT_SCRIPT}"
chmod +x ${WORKDIR}/${INIT_SCRIPT}

if [ "${SOURCED}" = "1" ]; then
  . ${WORKDIR}/${INIT_SCRIPT}

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
