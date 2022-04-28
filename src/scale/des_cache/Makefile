# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

ifndef REPO_HOME
  $(error "execute 'source ./bin/conf.sh' to configure environment")
endif
ifndef REPO_VERSION
  $(error "execute 'source ./bin/conf.sh' to configure environment")
endif

# =============================================================================

CID          = DES_CACHE

include util.mk

CID_SOURCES  = target.c util.c conv.c des.c
CID_HEADERS  = target.h util.h conv.h des.h

  GCC_PATHS += -L ${BUILD}/d4-7 -L ${BUILD}/DES-OUTE
  GCC_PATHS += -I ${BUILD}/d4-7 -I ${BUILD}/DES-OUTE
  GCC_LIBS  += -ld4

vpath %.h ${REPO_HOME}/build/DES-OUTE
vpath %.c ${REPO_HOME}/build/DES-OUTE

# -----------------------------------------------------------------------------

${BUILD}/%.D : ${BUILD}/%.conf_arg ${CID_SOURCES} ${CID_HEADERS}
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_D=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	@$(call FIXUP_SYMBOLS, ${@})

${BUILD}/%.R : ${BUILD}/%.conf_arg ${CID_SOURCES} ${CID_HEADERS}
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_R=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	@$(call FIXUP_SYMBOLS, ${@})

# -----------------------------------------------------------------------------

deps-fetch-hook :
	@if [ ! -e '${BUILD}/DES-OUTE.zip' ]                                                                                 ; then \
           curl 'https://www.schneier.com/sccd/DES-OUTE.ZIP'            --output '${BUILD}/DES-OUTE.ZIP' --silent --location ;      \
         fi
	@if [ ! -e '${BUILD}/d4-7.tar.gz'  ]                                                                                 ; then \
           curl 'https://ftp.cs.wisc.edu/markhill/DineroIV/d4-7.tar.gz' --output '${BUILD}/d4-7.tar.gz'  --silent --location ;      \
         fi

	@if ( ! ( cd ${BUILD} && shasum --algorithm '256' --status --check '${CURDIR}/build.hash' ) )                        ; then \
           exit 1                                                                                                            ;      \
         fi

	@if [ ! -d '${BUILD}/DES-OUTE'     ]                                                                                 ; then \
           unzip -qq -d ${BUILD}/DES-OUTE ${BUILD}/DES-OUTE.ZIP                                                              ;      \
         fi
	@if [ ! -d '${BUILD}/d4-7'         ]                                                                                 ; then \
           tar --extract --gzip --file='${BUILD}/d4-7.tar.gz' --directory='${BUILD}'                                         ;      \
         fi

deps-patch-hook :
	@patch --quiet --directory='${BUILD}/DES-OUTE' --strip='1' < build.patch

deps-build-hook :
	@cd ${BUILD}/d4-7 && ./configure --quiet && make --quiet

deps-clean-hook :
	@rm --force --recursive ${BUILD}/DES-OUTE.ZIP 
	@rm --force --recursive ${BUILD}/DES-OUTE

	@rm --force --recursive ${BUILD}/d4-7.tar.gz
	@rm --force --recursive ${BUILD}/d4-7

# =============================================================================
