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

CID          = AES_POWER

include util.mk

ifeq (${SCALE_CONF_${CID}_VERSION},0)
CID_SOURCES  = target_ecb.c util.c conv.c
CID_HEADERS  = target_ecb.h util.h conv.h
endif
ifeq (${SCALE_CONF_${CID}_VERSION},1)
CID_SOURCES  = target_ecb.c util.c conv.c
CID_HEADERS  = target_ecb.h util.h conv.h
endif
ifeq (${SCALE_CONF_${CID}_VERSION},2)
CID_SOURCES  = target_xts.c util.c conv.c
CID_HEADERS  = target_xts.h util.h conv.h
endif

  GCC_PATHS += -L ${BUILD}
  GCC_PATHS += -I ${BUILD}
  GCC_LIBS  +=

vpath %.h ${BUILD}
vpath %.c ${BUILD}

# -----------------------------------------------------------------------------

I8051_SOURCES     = wrapper.c                                      $(wildcard i8051*.c   ) 
I8051_HEADERS     = wrapper.h                                      $(wildcard i8051*.h   )
I8051_SOURCES_M4  = $(addprefix ${BUILD}/, $(patsubst %.m4_c, %.c, $(wildcard i8051*.m4_c)))
I8051_HEADERS_M4  = $(addprefix ${BUILD}/, $(patsubst %.m4_h, %.h, $(wildcard i8051*.m4_h)))

${I8051_SOURCES_M4} : ${BUILD}/%.c : i8051.m4 %.m4_c
	@m4 ${^} > ${@}
${I8051_HEADERS_M4} : ${BUILD}/%.h : i8051.m4 %.m4_h
	@m4 ${^} > ${@}

${BUILD}/aes.S   :          aes.m4_S
	@m4             ${<}                                                                            > ${@}
${BUILD}/aes.hex : ${BUILD}/aes.S
	@as31 -Fhex  -s ${<}                                                                            > ${@}
${BUILD}/aes.bin : ${BUILD}/aes.S
	@as31 -Fbyte -s ${<} | cut -d' ' -f2 | awk 'NR > 1 { printf( "," ) } { printf( "0x%s", $$1 ) }' > ${@}

CID_SOURCES += ${I8051_SOURCES} ${I8051_SOURCES_M4} ${BUILD}/aes.bin
CID_HEADERS += ${I8051_HEADERS} ${I8051_HEADERS_M4}

# -----------------------------------------------------------------------------

${BUILD}/%.T : ${BUILD}/%.conf_arg ${CID_SOURCES} ${CID_HEADERS} 
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_D=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	@$(call FIXUP_SYMBOLS, ${@})

${BUILD}/%.R : ${BUILD}/%.conf_arg ${CID_SOURCES} ${CID_HEADERS}
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_R=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	@$(call FIXUP_SYMBOLS, ${@})

# =============================================================================
