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

CID = AES_POWER

include libbuild.mk

I8051_SOURCES    =                         $(wildcard wrapper.c i8051*.c   )
I8051_HEADERS    =                         $(wildcard wrapper.h i8051*.h   )
I8051_SOURCES_M4 = $(patsubst %.m4_c, %.c, $(wildcard           i8051*.m4_c))
I8051_HEADERS_M4 = $(patsubst %.m4_h, %.h, $(wildcard           i8051*.m4_h))

aes.S   : aes.m4_S
	@m4             ${<}                                                                            > ${@}
aes.hex : aes.S
	@as31 -Fhex  -s ${<}                                                                            > ${@}
aes.bin : aes.S
	@as31 -Fbyte -s ${<} | cut -d' ' -f2 | awk 'NR > 1 { printf( "," ) } { printf( "0x%s", $$1 ) }' > ${@}

${I8051_SOURCES_M4} : %.c : i8051.m4 %.m4_c
	@m4 ${^} > ${@}
${I8051_HEADERS_M4} : %.h : i8051.m4 %.m4_h
	@m4 ${^} > ${@}

ifeq (${SCALE_CONF_${CID}_VERSION},0)
      TARGETS    = $(wildcard target_ecb.[ch])
endif
ifeq (${SCALE_CONF_${CID}_VERSION},1)
      TARGETS    = $(wildcard target_ecb.[ch])
endif
ifeq (${SCALE_CONF_${CID}_VERSION},2)
      TARGETS    = $(wildcard target_xts.[ch])
endif

${CURDIR}/build/%.D : ${TARGETS} ${CURDIR}/build/%.conf_arg aes.bin ${I8051_SOURCES} ${I8051_HEADERS} ${I8051_SOURCES_M4} ${I8051_HEADERS_M4} 
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_D=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	$(call FIXUP_SYMBOLS, ${@})

${CURDIR}/build/%.R : ${TARGETS} ${CURDIR}/build/%.conf_arg aes.bin ${I8051_SOURCES} ${I8051_HEADERS} ${I8051_SOURCES_M4} ${I8051_HEADERS_M4} 
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_R=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	$(call FIXUP_SYMBOLS, ${@})

clean-hook :
	@rm --force --recursive ${I8051_SOURCES_M4} 
	@rm --force --recursive ${I8051_HEADERS_M4}

	@rm --force --recursive aes.S aes.hex aes.bin

# =============================================================================
