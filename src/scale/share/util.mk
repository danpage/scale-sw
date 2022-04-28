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

define TOUPPER
  $(shell echo '${1}' | tr '[:lower:]' '[:upper:]')
endef
define TOLOWER
  $(shell echo '${1}' | tr '[:upper:]' '[:lower:]')
endef

define FIXUP_SYMBOLS
  @objcopy --only-keep-debug ${1} $(strip ${1})_debug
  @strip   --strip-all ${1}
  @objcopy --add-gnu-debuglink='$(strip ${1})_debug' ${1}
endef

# -----------------------------------------------------------------------------

BUILD           = ${REPO_HOME}/build/$(strip $(call TOLOWER, ${CID}))

BUILD_GLOB_DESC = %.pdf
BUILD_GLOB_CONF = %.conf %.conf_bin %.conf_txt %.conf_arg
BUILD_GLOB_EXAM = %.exam
BUILD_GLOB_TAR  = %.tar

ARCHIVE_GLOB    = ${SCALE_CONF_${CID}_ARCHIVE_GLOB}
ARCHIVE_PATH    = ${SCALE_CONF_${CID}_ARCHIVE_PATH}

vpath %.h ${REPO_HOME}/src/scale/share
vpath %.c ${REPO_HOME}/src/scale/share

# -----------------------------------------------------------------------------

${BUILD}          :
	@mkdir --parents ${@}

${BUILD}/desc.tex : desc.tex
	@TEXINPUTS="${REPO_HOME}/src/scale/share:${CURDIR}" latexpand ${REPO_HOME}/src/scale/share/desc.template | m4 ${M4_PATHS} ${M4_FLAGS} -DCID='${CID}' $(shell cat ${REPO_HOME}/build/conf.arg) util.m4 - > ${@}

${BUILD}/desc.pdf : ${BUILD}/desc.tex
	@for i in $$(seq 3)                                                                                                 ; do   \
           ${PDFLATEX_ENV} pdflatex ${PDFTEX_FLAGS} -halt-on-error -output-directory ${BUILD} ${BUILD}/desc.tex > /dev/null ;      \
           if [ -n "$$(find ${BUILD} -maxdepth 1 -name '*.bcf')" ]                                                          ; then \
             ${BIBER_ENV} biber ${BIBER_FLAGS} ${BUILD}/desc                                                    > /dev/null ;      \
           fi                                                                                                                      \
         done

${BUILD_GLOB_DESC} : ${BUILD}/desc.pdf
	@cp ${<} ${@}
${BUILD_GLOB_CONF} :
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${REPO_HOME}/src/scale/share/build_user.py --output='${BUILD}' --conf='${CONF}' --mode='conf' --path='${CURDIR}' --cid='${CID}' --uid='${*F}'
${BUILD_GLOB_EXAM} :
	@${PYTHON_ENV} python3 ${PYTHON_FLAGS} ${REPO_HOME}/src/scale/share/build_user.py --output='${BUILD}' --conf='${CONF}' --mode='exam' --path='${CURDIR}' --cid='${CID}' --uid='${*F}'
${BUILD_GLOB_TAR}  : ${ARCHIVE_GLOB}
	@tar --create --absolute-names --transform='s|${BUILD}|${*F}/${ARCHIVE_PATH}|' --file='${@}' ${^}

# -----------------------------------------------------------------------------

.PRECIOUS : ${BUILD_GLOB_DESC} ${BUILD_GLOB_CONF} ${BUILD_GLOB_EXAM} ${BUILD_GLOB_TAR}

deps-fetch      :
deps-fetch      : ${BUILD} deps-fetch-hook
deps-patch-hook :
deps-patch      : ${BUILD} deps-patch-hook
deps-build-hook :
deps-build      : ${BUILD} deps-build-hook
deps-clean-hook :
deps-clean      : ${BUILD} deps-clean-hook

     build-hook : 
     build      : ${BUILD}      build-hook ${BUILD}/desc.tex $(foreach P, ${ARCHIVE_GLOB} %.tar,$(addprefix ${BUILD}/, $(patsubst %, ${P}, ${SCALE_CONF_USERS})))

     clean-hook :
     clean      :               clean-hook
	@rm --force --recursive ${BUILD}

	@rm --force *.pyc
	@rm --force *.pyo

# =============================================================================
