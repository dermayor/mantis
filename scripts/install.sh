#!/bin/bash
set -x
set -e

function pdep() {
  echo "Instaling dependency: $1"
}

intall_python_flag="" # zero this in case system set for some reason

# Use sudo only if not root
if [ "$(id -u)" -eq 0 ]; then
  SUDO=""
else
  SUDO="sudo"
fi

echo "Installing mantis..."
git clone https://github.com/dermayor/mantis.git
cd mantis
git checkout cmake_refactor

$SUDO chmod +x ./scripts/install_deps.sh
$SUDO ./scripts/install_deps.sh

if [ "${1}" == "python" ]; then
  pdep "pybind11"
  $SUDO apt install pybind11-dev python3-pybind11
  intall_python_flag="-DPYTHON=True"
fi

mkdir -p build && cd build && cmake .. ${intall_python_flag} && $SUDO make -j $(nproc) install

echo "Cleaning up..."
cd ../.. && rm -rf mantis

$SUDO ldconfig

echo "Mantis installation complete"
