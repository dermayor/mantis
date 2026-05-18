#!/bin/bash
set -x
set -e

function pdep() {
  echo "Instaling dependency: $1"
}

# Use sudo only if not root
if [ "$(id -u)" -eq 0 ]; then
  SUDO=""
else
  SUDO="sudo"
fi

echo "installing basic dependencies..."
$SUDO apt-get update && $SUDO apt-get install -y \
  build-essential \
  cmake

#nlohmann_json
pdep "nlohmann_json"
$SUDO apt-get install -y nlohmann-json3-dev

# pybind11
pdep "pybind11"
$SUDO apt-get install -y python3-pybind11 pybind11-dev

# boos po
pdep "boost program options"
$SUDO apt-get install -y libboost-program-options-dev
