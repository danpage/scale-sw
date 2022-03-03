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

 ifndef CONF
 export CONF            = ${CURDIR}/scale.conf
 endif

 export DIR_ROOT        = ${CURDIR}
 export DIR_CONF        = $(shell dirname ${CONF})

# In reality this is a hack, motivated by a (somewhat) circular problem: we
# want to include configuration distilled from ${CONF}, but distillation is
# dependent on the shared libraries.  If we allow said configuration to be
# implicitly rebuilt (provoked by include), this yields a mess.  Probably a
# cleaner solution would be to separate the tasks from each other, i.e., in
# separate Makefiles.

ifneq ($(strip $(wildcard ${DIR_ROOT}/build/conf.mk)),)
 include ${DIR_ROOT}/build/conf.mk
endif

 export      GCC_FLAGS += -std=gnu99 -m64 -O3 -g -ggdb -Wl,--build-id=sha1
 export      GCC_PATHS += -L${DIR_ROOT}/src/share/target/build/lib
 export      GCC_PATHS += -I${DIR_ROOT}/src/share/target/build/include
 export      GCC_LIBS  += -ltarget -lcrypto -lgmp

 ifdef STATIC
 export      GCC_FLAGS += -static -static-libgcc
 export      GCC_LIBS  += -ldl 
 endif

 export     MAKE_FLAGS +=
 export     MAKE_PATHS += -I${DIR_ROOT}/src/share/build/build/include

 export       M4_FLAGS +=
 export       M4_PATHS += -I${DIR_ROOT}/src/share/build/build/include

 export   PYTHON_FLAGS +=                 -O
 export   PYTHON_PATHS +=   ${DIR_ROOT}/src/share/build/build
 export   PYTHON_ENV   += PYTHONPATH="${PYTHONPATH}:${PYTHON_PATHS}"

 export PDFLATEX_FLAGS += -shell-escape
 export PDFLATEX_PATHS +=   ${DIR_ROOT}/src/share/build/build/include:${DIR_ROOT}/build
 export PDFLATEX_ENV   += TEXINPUTS="${TEXINPUTS}:${PDFLATEX_PATHS}"

 export    BIBER_FLAGS += 
 export    BIBER_PATHS +=   ${DIR_ROOT}/src/share/build/build/include:${DIR_ROOT}/build
 export    BIBER_ENV   += BIBINPUTS="${BIBINPUTS}:${BIBER_PATHS}"

 export ARCHIVES     += $(patsubst %, ${DIR_ROOT}/build/%.tar.gz, ${SCALE_CONF_USERS})

# -----------------------------------------------------------------------------

.PHONY : build

%-share     :
	@$(foreach DIR, $(wildcard ${DIR_ROOT}/src/share/*), make ${MAKE_PATHS} ${MAKE_FLAGS} -C ${DIR} ${*} ;)
%-challenge :
	@$(foreach DIR, ${SCALE_CONF_CHALLENGES},            make ${MAKE_PATHS} ${MAKE_FLAGS} -C ${DIR} ${*} ;)

${DIR_ROOT}/build/conf.mk  : ${CONF}
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${DIR_ROOT}/src/share/build/build/libbuild/build_conf.py --path='${CURDIR}' --conf='${CONF}' --mode='mk'
${DIR_ROOT}/build/conf.sh  : ${CONF}
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${DIR_ROOT}/src/share/build/build/libbuild/build_conf.py --path='${CURDIR}' --conf='${CONF}' --mode='sh'
${DIR_ROOT}/build/conf.arg : ${CONF}
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${DIR_ROOT}/src/share/build/build/libbuild/build_conf.py --path='${CURDIR}' --conf='${CONF}' --mode='arg'

${ARCHIVES} : ${DIR_ROOT}/build/%.tar.gz :
	@$(foreach DIR, ${SCALE_CONF_CHALLENGES}, tar --concatenate --file=${DIR_ROOT}/build/${*F}.tar ${DIR}/build/${*F}.tar ;) gzip ${DIR_ROOT}/build/${*}.tar

build-deps  : fetch-deps-challenge patch-deps-challenge build-deps-challenge

build-conf  : ${DIR_ROOT}/build/conf.mk ${DIR_ROOT}/build/conf.sh ${DIR_ROOT}/build/conf.arg

build-dist  : ${ARCHIVES}

clean-deps  : clean-deps-challenge

clean-conf  :
	@rm --force ${DIR_ROOT}/build/conf.mk
	@rm --force ${DIR_ROOT}/build/conf.sh
	@rm --force ${DIR_ROOT}/build/conf.arg

clean-dist  :
	@rm --force ${DIR_ROOT}/build/*.tar.gz

build-step1 : build-share build-conf 
build-step2 : build-deps build-challenge build-dist 

clean-step1 : clean-deps clean-challenge clean-dist
clean-step2 : clean-share clean-conf

venv        : ${REPO_HOME}/requirements.txt
	@${REPO_HOME}/bin/venv.sh

spotless    : clean-step1 clean-step2
	@rm --force --recursive ${REPO_HOME}/build/*

# =============================================================================
