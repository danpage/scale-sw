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

    DIRS   = ./build/libbuild
    DIRS  += ./build/include

 SOURCES   = $(wildcard *.py )
 SOURCES  += $(wildcard *.mk )
 SOURCES  += $(wildcard *.m4 )
 SOURCES  += $(wildcard *.tex)
 SOURCES  += $(wildcard *.sty)
 SOURCES  += $(wildcard *.bib)

 TARGETS   = $(addprefix ./build/libbuild/, $(notdir $(filter %.py,  ${SOURCES})))
 TARGETS  += $(addprefix ./build/include/,  $(notdir $(filter %.mk,  ${SOURCES})))
 TARGETS  += $(addprefix ./build/include/,  $(notdir $(filter %.m4,  ${SOURCES})))
 TARGETS  += $(addprefix ./build/include/,  $(notdir $(filter %.tex, ${SOURCES})))
 TARGETS  += $(addprefix ./build/include/,  $(notdir $(filter %.sty, ${SOURCES})))
 TARGETS  += $(addprefix ./build/include/,  $(notdir $(filter %.bib, ${SOURCES})))

# -----------------------------------------------------------------------------

${DIRS} :
	@mkdir --parents ${@}

$(filter %.py,  ${TARGETS}) : ./build/libbuild/%.py : ${SOURCES}
	@cp $(filter %$(notdir ${@}), ${^}) ${@}
$(filter %.mk,  ${TARGETS}) : ./build/include/%.mk  : ${SOURCES}
	@cp $(filter %$(notdir ${@}), ${^}) ${@}
$(filter %.m4,  ${TARGETS}) : ./build/include/%.m4  : ${SOURCES}
	@cp $(filter %$(notdir ${@}), ${^}) ${@}
$(filter %.tex, ${TARGETS}) : ./build/include/%.tex : ${SOURCES}
	@cp $(filter %$(notdir ${@}), ${^}) ${@}
$(filter %.sty, ${TARGETS}) : ./build/include/%.sty : ${SOURCES}
	@cp $(filter %$(notdir ${@}), ${^}) ${@}
$(filter %.bib, ${TARGETS}) : ./build/include/%.bib : ${SOURCES}
	@cp $(filter %$(notdir ${@}), ${^}) ${@}

.PHONY : build

build  : ${DIRS} ${TARGETS}

clean  :
	@rm --force --recursive ./build/*

# =============================================================================
