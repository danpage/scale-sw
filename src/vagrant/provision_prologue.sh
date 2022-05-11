#!/bin/bash

# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

# =============================================================================

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
sudo apt --quiet --assume-yes install ntp
sudo apt --quiet --assume-yes install openssl
sudo apt --quiet --assume-yes install python3
sudo apt --quiet --assume-yes install python3-pip
sudo apt --quiet --assume-yes install python3-venv
sudo apt --quiet --assume-yes install python3-wheel
sudo apt --quiet --assume-yes install texlive-full
sudo apt --quiet --assume-yes install wget
sudo apt --quiet --assume-yes install xauth

sudo systemctl enable  ntp 
sudo systemctl restart ntp

# =============================================================================
