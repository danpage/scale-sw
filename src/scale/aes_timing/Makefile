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

CID          = AES_TIMING

include util.mk

CID_SOURCES  = target.c util.c conv.c aes.c
CID_HEADERS  = target.h util.h conv.h aes.h

  GCC_PATHS += -L ${BUILD}
  GCC_PATHS += -I ${BUILD}
  GCC_LIBS  +=

vpath %.h ${BUILD}
vpath %.c ${BUILD}

# -----------------------------------------------------------------------------

${BUILD}/%.T : ${BUILD}/%.conf_arg ${CID_SOURCES} ${CID_HEADERS}
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_T=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	@$(call FIXUP_SYMBOLS, ${@})

${BUILD}/%.R : ${BUILD}/%.conf_arg ${CID_SOURCES} ${CID_HEADERS}
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_R=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	@$(call FIXUP_SYMBOLS, ${@})

# =============================================================================
