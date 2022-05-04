#!/bin/bash

# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

# =============================================================================

# system: install dependencies

sudo apt --quiet --assume-yes update
sudo apt --quiet --assume-yes upgrade
        
sudo apt --quiet --assume-yes install as31
sudo apt --quiet --assume-yes install gcc 
sudo apt --quiet --assume-yes install git
sudo apt --quiet --assume-yes install git-lfs
sudo apt --quiet --assume-yes install libgmp-dev
sudo apt --quiet --assume-yes install libssl-dev
sudo apt --quiet --assume-yes install libtool
sudo apt --quiet --assume-yes install linux-image-extra-virtual
sudo apt --quiet --assume-yes install make
sudo apt --quiet --assume-yes install m4
sudo apt --quiet --assume-yes install openssl
sudo apt --quiet --assume-yes install python3
sudo apt --quiet --assume-yes install python3-pip
sudo apt --quiet --assume-yes install python3-venv
sudo apt --quiet --assume-yes install python3-wheel
sudo apt --quiet --assume-yes install texlive-full
sudo apt --quiet --assume-yes install wget
sudo apt --quiet --assume-yes install xauth

# -----------------------------------------------------------------------------

# SCALE: clone repo.

git clone https://github.com/danpage/scale-sw.git /home/vagrant/scale-sw
cd /home/vagrant/scale-sw
git submodule update --init --recursive 
source ./bin/conf.sh 

# SCALE: build virtual environment

make venv && source ${REPO_HOME}/build/venv/bin/activate

# SCALE: build challenge material

make --directory="${REPO_HOME}/src/scale" build-step1
make --directory="${REPO_HOME}/src/scale" build-step2

for ARCHIVE in $(ls -1 ${REPO_HOME}/build/*.tar.gz) ; do
  tar --directory="${REPO_HOME}/build" --extract --file ${ARCHIVE}
done

# SCALE: fix-up ownership

sudo chown --recursive vagrant:vagrant /home/vagrant/scale-sw

# -----------------------------------------------------------------------------

# system: clean-up
sudo apt clean

# =============================================================================
