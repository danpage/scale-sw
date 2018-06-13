# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

define CHECK_DEPS_BASH
  @if (     which "$(strip ${1})"                                                                  &> /dev/null ) ; then \
     echo "+ deps-check passed : $(strip ${1})"                                                                   ;      \
   else                                                                                                                  \
     echo "- deps-check failed : $(strip ${1})"                                                                   ;      \
   fi
endef

define CHECK_DEPS_PYTHON
  @if ( python -c 'import pkg_resources ; pkg_resources.require( "$(strip ${1})>=$(strip ${2})" )' &> /dev/null ) ; then \
     echo "+ deps-check passed : $(strip ${1})"                                                                   ;      \
   else                                                                                                                  \
     echo "- deps-check failed : $(strip ${1})"                                                                   ;      \
   fi
endef

define CHECK_DEPS_PDFLATEX
  @if ( kpsewhich "$(strip ${1})"                                                                  &> /dev/null ) ; then \
     echo "+ deps-check passed : $(strip ${1})"                                                                   ;      \
   else                                                                                                                  \
     echo "- deps-check failed : $(strip ${1})"                                                                   ;      \
   fi
endef

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

.PHONY   : build

%-share     :
	@$(foreach DIR, $(wildcard ${DIR_ROOT}/src/share/*), make ${MAKE_PATHS} ${MAKE_FLAGS} -C ${DIR} ${*} ;)
%-challenge :
	@$(foreach DIR, ${SCALE_CONF_CHALLENGES},            make ${MAKE_PATHS} ${MAKE_FLAGS} -C ${DIR} ${*} ;)

${DIR_ROOT}/build/conf.mk  : ${CONF}
	@${PYTHON_ENV} python ${PYTHON_FLAGS} ${DIR_ROOT}/src/share/build/build/libbuild/build_conf.py --path='${CURDIR}' --conf='${CONF}' --mode='mk'
${DIR_ROOT}/build/conf.sh  : ${CONF}
	@${PYTHON_ENV} python ${PYTHON_FLAGS} ${DIR_ROOT}/src/share/build/build/libbuild/build_conf.py --path='${CURDIR}' --conf='${CONF}' --mode='sh'
${DIR_ROOT}/build/conf.arg : ${CONF}
	@${PYTHON_ENV} python ${PYTHON_FLAGS} ${DIR_ROOT}/src/share/build/build/libbuild/build_conf.py --path='${CURDIR}' --conf='${CONF}' --mode='arg'

${ARCHIVES} : ${DIR_ROOT}/build/%.tar.gz :
	@$(foreach DIR, ${SCALE_CONF_CHALLENGES}, tar --concatenate --file=${DIR_ROOT}/build/${*F}.tar ${DIR}/build/${*F}.tar ;) gzip ${DIR_ROOT}/build/${*}.tar

check-deps  :
	@$(call CHECK_DEPS_BASH,     as31)
	@$(call CHECK_DEPS_BASH,    biber)
	@$(call CHECK_DEPS_BASH,  doxygen)
	@$(call CHECK_DEPS_BASH,      gcc)
	@$(call CHECK_DEPS_BASH,     gzip)
	@$(call CHECK_DEPS_BASH,       m4)
	@$(call CHECK_DEPS_BASH,     make)
	@$(call CHECK_DEPS_BASH,    patch)
	@$(call CHECK_DEPS_BASH, pdflatex)
	@$(call CHECK_DEPS_BASH,   python)
	@$(call CHECK_DEPS_BASH,   shasum)
	@$(call CHECK_DEPS_BASH,    unzip)
	@$(call CHECK_DEPS_BASH,     wget)
	@$(call CHECK_DEPS_BASH,      zip)

	@$(call CHECK_DEPS_PYTHON, pycrypto,    2.6.1)
	@$(call CHECK_DEPS_PYTHON, braceexpand, 0.1.2)

	@$(call CHECK_DEPS_PDFLATEX, algorithm2e.sty)
	@$(call CHECK_DEPS_PDFLATEX,     amsmath.sty)
	@$(call CHECK_DEPS_PDFLATEX,     amssymb.sty)
	@$(call CHECK_DEPS_PDFLATEX,    biblatex.sty)
	@$(call CHECK_DEPS_PDFLATEX,    fancyhdr.sty)
	@$(call CHECK_DEPS_PDFLATEX,     fontenc.sty)
	@$(call CHECK_DEPS_PDFLATEX,    geometry.sty)
	@$(call CHECK_DEPS_PDFLATEX,    hyperref.sty)
	@$(call CHECK_DEPS_PDFLATEX,    listings.sty)
	@$(call CHECK_DEPS_PDFLATEX,     siunitx.sty)
	@$(call CHECK_DEPS_PDFLATEX,  standalone.cls)
	@$(call CHECK_DEPS_PDFLATEX,    titlesec.sty)
	@$(call CHECK_DEPS_PDFLATEX,       xargs.sty)
	@$(call CHECK_DEPS_PDFLATEX,     xstring.sty)

build-deps  : fetch-deps-challenge patch-deps-challenge build-deps-challenge

build-conf  : ${DIR_ROOT}/build/conf.mk ${DIR_ROOT}/build/conf.sh ${DIR_ROOT}/build/conf.arg

build-dist  : ${ARCHIVES}

build-doc   :
	@doxygen

clean-deps  : clean-deps-challenge

clean-conf  :
	@rm -f  ${DIR_ROOT}/build/conf.mk
	@rm -f  ${DIR_ROOT}/build/conf.sh
	@rm -f  ${DIR_ROOT}/build/conf.arg

clean-doc   :
	@rm -fr ${DIR_ROOT}/build/doc

clean-dist  :
	@rm -f  ${DIR_ROOT}/build/*.tar.gz

build-step1 : build-share build-conf 
build-step2 : build-doc build-deps build-challenge build-dist 

clean-step1 : clean-doc clean-deps clean-challenge clean-dist
clean-step2 : clean-share clean-conf

spotless    : clean-step1 clean-step2
	@rm -rf ${DIR_ROOT}/build/*
