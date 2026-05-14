#!/bin/bash
set -x
set -e

# Use sudo only if not root
if [ "$(id -u)" -eq 0 ]; then
  SUDO=""
else
  SUDO="sudo"
fi

echo "Installing mantis..."
git clone https://github.com/dermayor/mantis.git
cd mantis

$SUDO chmod +x ./scripts/install_deps.sh
$SUDO ./install_deps.sh

mkdir -p build && cd build && cmake .. && $SUDO make -j $(nproc) install

echo "Cleaning up..."
cd ../.. && rm -rf mantis

$SUDO ldconfig

echo "Mantis installation complete"
