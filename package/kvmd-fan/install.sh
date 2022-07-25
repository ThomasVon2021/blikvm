#!/bin/bash

mkdir -p "/usr/bin"
install -Dm755 kvmd-fan.py "/usr/bin/kvmd-fan"

mkdir -p "/usr/lib/systemd/system"
cp kvmd-fan.service "/usr/lib/systemd/system"

echo "install fan ok"
