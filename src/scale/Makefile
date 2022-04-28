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

ifneq "$(strip $(wildcard ${REPO_HOME}/build/conf.mk))" ""
include ${REPO_HOME}/build/conf.mk
endif

# -----------------------------------------------------------------------------

export CONF           ?= ${REPO_HOME}/src/scale/scale.conf

export      GCC_FLAGS += -std='gnu99' -m64 -O3 -g -ggdb -Wl,--build-id='sha1'
export      GCC_PATHS += -L ${REPO_HOME}/src/scale/share/target/build/lib
export      GCC_PATHS += -I ${REPO_HOME}/src/scale/share/target/build/include
export      GCC_LIBS  += -ltarget -lcrypto -lgmp

ifdef STATIC
export      GCC_FLAGS += -static -static-libgcc
export      GCC_LIBS  += -ldl 
endif

export     MAKE_FLAGS +=
export     MAKE_PATHS += -I ${REPO_HOME}/src/scale/share/build/build/include

export       M4_FLAGS +=
export       M4_PATHS += -I ${REPO_HOME}/src/scale/share/build/build/include

export   PYTHON_FLAGS +=                   -O
export   PYTHON_PATHS +=    ${REPO_HOME}/src/scale/share/build/build
export   PYTHON_ENV   += PYTHONPATH="${PYTHONPATH}:${PYTHON_PATHS}"

export PDFLATEX_FLAGS += -shell-escape
export PDFLATEX_PATHS +=    ${REPO_HOME}/src/scale/share/build/build/include:${REPO_HOME}/build
export PDFLATEX_ENV   += TEXINPUTS="${TEXINPUTS}:${PDFLATEX_PATHS}"

export    BIBER_FLAGS += 
export    BIBER_PATHS +=    ${REPO_HOME}/src/scale/share/build/build/include:${REPO_HOME}/build
export    BIBER_ENV   += BIBINPUTS="${BIBINPUTS}:${BIBER_PATHS}"

export ARCHIVES       += $(patsubst %, ${REPO_HOME}/build/%.tar.gz, ${SCALE_CONF_USERS})

# -----------------------------------------------------------------------------

%-share     :
	@$(foreach DIR, $(wildcard ${REPO_HOME}/src/scale/share/*), make ${MAKE_PATHS} ${MAKE_FLAGS} --directory='${DIR}' ${*} ;)
%-challenge :
	@$(foreach DIR, ${SCALE_CONF_CHALLENGES},                   make ${MAKE_PATHS} ${MAKE_FLAGS} --directory='${DIR}' ${*} ;)

${REPO_HOME}/build/conf.mk  : ${CONF}
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${REPO_HOME}/src/scale/share/build/build/libbuild/build_conf.py --path='${REPO_HOME}' --conf='${CONF}' --mode='mk'
${REPO_HOME}/build/conf.sh  : ${CONF}
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${REPO_HOME}/src/scale/share/build/build/libbuild/build_conf.py --path='${REPO_HOME}' --conf='${CONF}' --mode='sh'
${REPO_HOME}/build/conf.arg : ${CONF}
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${REPO_HOME}/src/scale/share/build/build/libbuild/build_conf.py --path='${REPO_HOME}' --conf='${CONF}' --mode='arg'

${ARCHIVES} : ${REPO_HOME}/build/%.tar.gz :
	@$(foreach DIR, ${SCALE_CONF_CHALLENGES}, tar --concatenate --file='${REPO_HOME}/build/${*F}.tar' ${DIR}/build/${*F}.tar ;) gzip ${REPO_HOME}/build/${*}.tar

build-deps  : fetch-deps-challenge patch-deps-challenge build-deps-challenge

build-conf  : ${REPO_HOME}/build/conf.mk ${REPO_HOME}/build/conf.sh ${REPO_HOME}/build/conf.arg

build-dist  : ${ARCHIVES}

clean-deps  : clean-deps-challenge

clean-conf  :
	@rm --force ${REPO_HOME}/build/conf.mk
	@rm --force ${REPO_HOME}/build/conf.sh
	@rm --force ${REPO_HOME}/build/conf.arg

clean-dist  :
	@rm --force ${REPO_HOME}/build/*.tar.gz

# -----------------------------------------------------------------------------

build-step1 : build-share build-conf 
build-step2 : build-deps build-challenge build-dist 

clean-step1 : clean-deps clean-challenge clean-dist
clean-step2 : clean-share clean-conf

spotless    :
	@rm --force --recursive ${REPO_HOME}/build/*

# =============================================================================
