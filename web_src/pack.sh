#!/bin/bash

# build client
cd web_client
npm install
npm run build
cp -r dist ../web_server
cd ../

# build server
cd ./web_server
npm install
npm run clean
npm run build
