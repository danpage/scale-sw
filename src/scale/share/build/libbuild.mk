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

BUILD_GLOB_DESC  = %.pdf
BUILD_GLOB_CONF  = %.conf %.conf_bin %.conf_txt %.conf_arg
BUILD_GLOB_EXAM  = %.exam
BUILD_GLOB_TAR   = %.tar

ARCHIVE_GLOB     = ${SCALE_CONF_${CID}_ARCHIVE_GLOB}
ARCHIVE_PATH     = ${SCALE_CONF_${CID}_ARCHIVE_PATH}

define FIXUP_SYMBOLS
  @objcopy --only-keep-debug ${1} $(strip ${1})_debug
  @strip   --strip-all ${1}
  @objcopy --add-gnu-debuglink='$(strip ${1})_debug' ${1}
endef

# -----------------------------------------------------------------------------

${CURDIR}/build/desc.tex  : desc.tex 
	@m4 ${M4_PATHS} ${M4_FLAGS} -DCID=${CID} $(shell cat ${DIR_ROOT}/build/conf.arg) libbuild.m4 ${<} > ${@}
${CURDIR}/build/desc.tikz : desc.tikz
	@m4 ${M4_PATHS} ${M4_FLAGS} -DCID=${CID} $(shell cat ${DIR_ROOT}/build/conf.arg) libbuild.m4 ${<} > ${@}

${CURDIR}/build/desc.pdf : ${CURDIR}/build/desc.tex ${CURDIR}/build/desc.tikz
	@for i in $$(seq 3)                                                                                                               ; do   \
           ${PDFLATEX_ENV} pdflatex ${PDFTEX_FLAGS} -halt-on-error -output-directory ${CURDIR}/build ${CURDIR}/build/desc.tex > /dev/null ;      \
           if [ -n "$$(find ${CURDIR}/build -maxdepth 1 -name '*.bcf')" ]                                                                 ; then \
             ${BIBER_ENV} biber ${BIBER_FLAGS} ${CURDIR}/build/desc                                                           > /dev/null ;      \
           fi                                                                                                                                    \
         done

${BUILD_GLOB_DESC} : ${CURDIR}/build/desc.pdf
	@cp ${<} ${@}
${BUILD_GLOB_CONF} :
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${DIR_ROOT}/src/share/build/build/libbuild/build_user.py --path='${CURDIR}' --conf='${CONF}' --mode='conf' --cid='${CID}' --uid='${*F}'
${BUILD_GLOB_EXAM} :
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${DIR_ROOT}/src/share/build/build/libbuild/build_user.py --path='${CURDIR}' --conf='${CONF}' --mode='exam' --cid='${CID}' --uid='${*F}'
${BUILD_GLOB_TAR}  : ${ARCHIVE_GLOB}
	@tar --create --absolute-names --transform='s|${CURDIR}/build|${*F}/${ARCHIVE_PATH}|' --file='${@}' ${^}

# -----------------------------------------------------------------------------

.PHONY : build

.PRECIOUS : ${BUILD_GLOB_DESC} ${BUILD_GLOB_CONF} ${BUILD_GLOB_EXAM} ${BUILD_GLOB_TAR}

fetch-deps-hook :
fetch-deps      : fetch-deps-hook
patch-deps-hook :
patch-deps      : patch-deps-hook
build-deps-hook :
build-deps      : build-deps-hook
clean-deps-hook :
clean-deps      : clean-deps-hook

build-hook : 
build      : build-hook ${CURDIR}/build/desc.tex ${CURDIR}/build/desc.tikz $(foreach P, ${ARCHIVE_GLOB} %.tar,$(addprefix ${CURDIR}/build/, $(patsubst %, ${P}, ${SCALE_CONF_USERS})))

clean-hook :
clean      : clean-hook
	@rm -rf ${CURDIR}/build/*

	@rm -f *.pyc
	@rm -f *.pyo

# =============================================================================
