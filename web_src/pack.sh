#!/bin/bash

# build client
cd web-client
npm install
npm run build
cp -r dist ../web-server
cd ../

# build server
cd ./web-server
npm run clean
npm run build
