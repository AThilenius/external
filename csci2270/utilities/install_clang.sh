#!/bin/bash

# Copyright 2015 Alec Thilenius
# All rights reserved.

# Added from dependency core/shell/log
RED='\033[0;31m'
YELLOW='\033[0;33m'
GREEN='\033[0;32m'
NC='\033[0m'

function INFO {
  printf "${GREEN}[INFO]${NC}: $1\n"
}

function WARN {
  printf "${YELLOW}[WARN]${NC}: $1\n"
}

function EROR {
  1>&2 printf "${RED}[EROR] $(date): $1${NC}\n"
  exit 1
}

# Base script
INFO 'Updating apt-get'
sudo apt-get update

INFO 'Installing Clang 3.6, libC++ and the lldb 3.6 debugger'
sudo apt-get install -y libc++-dev libc++abi-dev clang-3.6 lldb-3.6 || \
    EROR 'Failed to install Clang 3.6 :('

INFO 'Adding symlink for Clang'
if [ ! -L /usr/bin/clang ]; then
  sudo ln -s /usr/bin/clang-3.6 /usr/bin/clang || \
      EROR 'Failed to create symlinks :('
fi
if [ ! -L /usr/bin/clang++ ]; then
  sudo ln -s /usr/bin/clang++-3.6 /usr/bin/clang++ || \
      EROR 'Failed to create symlinks :('
fi

INFO 'Setting clang as the default compiler'
echo 'export CC=/usr/bin/clang' >> ~/.bashrc
echo 'export CXX=/usr/bin/clang++' >> ~/.bashrc
echo 'export CXXFLAGS=--std=c++11' >> ~/.bashrc

INFO 'Clang successfully installed!'
INFO 'You might need to restart your shell, or log off and log back in'
