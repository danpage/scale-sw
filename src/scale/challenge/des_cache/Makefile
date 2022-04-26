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

CID = DES_CACHE

include libbuild.mk

${CURDIR}/build/%.D : $(wildcard target.[ch]) ${CURDIR}/build/%.conf_arg
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_D=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	@$(call FIXUP_SYMBOLS, ${@})

${CURDIR}/build/%.R : $(wildcard target.[ch]) ${CURDIR}/build/%.conf_arg
	@gcc ${GCC_PATHS} ${GCC_FLAGS} -DCID=${CID} -DSCALE_CONF_${CID}_TARGET_R=1 $(shell cat ${REPO_HOME}/build/conf.arg $(filter %.conf_arg, ${^})) -o ${@} $(filter %.c, ${^}) ${GCC_LIBS}
	@$(call FIXUP_SYMBOLS, ${@})

GCC_PATHS += -Ld4-7
GCC_PATHS += -Id4-7
GCC_LIBS  += -ld4

fetch-deps-hook :
	@if [ ! -e 'DES-OUTE.zip' ]                                                                                 ; then \
           curl 'https://www.schneier.com/sccd/DES-OUTE.ZIP'            --output 'DES-OUTE.ZIP' --silent --location ;      \
         fi
	@if [ ! -e 'd4-7.tar.gz'  ]                                                                                 ; then \
           curl 'https://ftp.cs.wisc.edu/markhill/DineroIV/d4-7.tar.gz' --output 'd4-7.tar.gz'  --silent --location ;      \
         fi

	@if ( ! shasum --algorithm '256' --status --check 'build.hash' )                                            ; then \
           exit 1                                                                                                   ;      \
         fi

	@if [ ! -d 'DES-OUTE'     ]                                                                                 ; then \
           unzip -qq -d DES-OUTE DES-OUTE.ZIP                                                                       ;      \
         fi
	@if [ ! -d 'd4-7'         ]                                                                                 ; then \
           tar --extract --gzip --file='d4-7.tar.gz'                                                                ;      \
         fi

patch-deps-hook :
	@patch --quiet --directory='DES-OUTE' --strip='1' < build.patch

build-deps-hook :
	@cd d4-7 && ./configure --quiet && make --quiet

clean-deps-hook :
	@rm --force --recursive DES-OUTE.ZIP 
	@rm --force --recursive DES-OUTE

	@rm --force --recursive d4-7.tar.gz
	@rm --force --recursive d4-7

# =============================================================================
