#!/bin/bash

# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

# =============================================================================

# clone repo.

git clone https://github.com/danpage/scale-sw.git /home/vagrant/scale-sw
cd /home/vagrant/scale-sw
git submodule update --init --recursive 
source ./bin/conf.sh 

# build virtual environment

make venv && source ${REPO_HOME}/build/venv/bin/activate

# build challenge material

make --directory="${REPO_HOME}/src/scale" build-step1
make --directory="${REPO_HOME}/src/scale" build-step2

for ARCHIVE in $(ls -1 ${REPO_HOME}/build/*.tar.gz) ; do
  tar --directory="${REPO_HOME}/build" --extract --file ${ARCHIVE}
done

# fix-up ownership

sudo chown --recursive vagrant:vagrant /home/vagrant/scale-sw

# =============================================================================
