#!/bin/bash --norc

./create_nondemo.rb

cat <<EOF
----------------
 Build mrLiquid
----------------
EOF
installjammer --build-for-release mrLiquid.mpi || exit 1

cat <<EOF
---------------------
 Build mrLiquid-demo
---------------------
EOF
installjammer --build-for-release mrLiquid-demo.mpi

