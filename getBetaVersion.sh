#!/bin/bash
curl --silent https://releases.domoticz.com/releases/beta/version_linux_x86_64.h | awk '{print $2"="$3}' > beta.env
source beta.env
git checkout -b 3.$APPVERSION-beta
sed -i "s/ARG BRANCH_NAME/ENV BUILD_BRANCH=$APPHASH/g" Dockerfile
git add Dockerfile
git commit -m"beta(): bump to 3.$APPVERSION-beta"