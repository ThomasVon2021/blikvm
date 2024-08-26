#!/bin/bash

# build client
cd web-client
npm install
npm run build
cp -r dist ../web_server
cd ../

# build server
cd ./web_server
npm run clean
npm run build
