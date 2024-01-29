#!/bin/bash --norc

./create_symlinks.rb && ./build.sh && ./zip.sh
