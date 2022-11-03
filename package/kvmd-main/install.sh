#!/bin/bash

mkdir -p "/usr/bin"
cp ../../src/demo.bin /usr/bin/kvmd-main

mkdir -p "/usr/lib/systemd/system"
cp kvmd-main.service "/usr/lib/systemd/system"

echo "install ustreamer ok"