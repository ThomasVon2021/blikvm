#!/bin/bash

mkdir -p "/usr/lib/systemd/system"
cp kvmd-janus.service "/usr/lib/systemd/system"

echo "install janus ok"