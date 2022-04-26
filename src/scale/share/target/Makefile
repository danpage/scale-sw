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

    DIRS   = ./build/lib 
    DIRS  += ./build/include/target

INCLUDES   = . 

 HEADERS   = $(wildcard *.h)
 SOURCES   = $(wildcard *.c)

 OBJECTS   = $(addprefix ./build/lib/,            $(notdir $(patsubst %.c, %.o, ${SOURCES})))

 TARGETS   = $(addprefix ./build/include/target/, $(notdir $(patsubst %.h, %.h, ${HEADERS})))
 TARGETS  += ./build/lib/libtarget.a

# -----------------------------------------------------------------------------

${DIRS} :
	@mkdir --parents ${@}

./build/lib/%.o : %.c %.h
	@gcc $(patsubst %, -I %, ${INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} -c -o ${@} ${<}
./build/lib/%.o : %.S
	@gcc $(patsubst %, -I %, ${INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} -c -o ${@} ${<}

$(filter %.h,  ${TARGETS}) : ./build/include/%.h :                ${HEADERS}
	@cp $(filter %$(notdir ${@}), ${^}) ${@}
$(filter %.a,  ${TARGETS}) : ./build/lib/%.a     :                ${OBJECTS}
	@ar rcs ${@} ${^}
$(filter %.ld, ${TARGETS}) : ./build/lib/%.ld    : $(filter %.ld, ${SCRIPTS})
	@cp ${<} ${@} 
$(filter %.mk, ${TARGETS}) : ./build/lib/%.mk    : $(filter %.mk, ${SCRIPTS})
	@cp ${<} ${@} 

.PHONY : build

build  : ${DIRS} ${TARGETS}

clean  :
	@rm --force --recursive ./build/*

# =============================================================================
