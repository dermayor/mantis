#!/bin/bash
set -x
set -e

echo "Installing mantis..."
git clone https://github.com/dermayor/mantis.git
cd mantis && ./scripts/install_deps.sh

mkdir -p build && cd build && cmake .. && $SUDO make -j $(nproc) install

echo "Cleaning up..."
cd ../.. && rm -rf mantis

$SUDO ldconfig

echo "Mantis installation complete"
