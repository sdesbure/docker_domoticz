#!/bin/bash
curl --silent https://releases.domoticz.com/releases/beta/version_linux_x86_64.h | awk '{print $2"="$3}' > beta.env
source beta.env
rm beta.env
git branch -r | grep "3.$APPVERSION-beta"
EXIST=$?
if [[ $EXIST != "0" ]]; then
git checkout -b 3.$APPVERSION-beta
sed -i "s/ARG APP_HASH/ENV APP_HASH=$APPHASH/g" Dockerfile
sed -i "s/ARG BRANCH_NAME/ENV BUILD_BRANCH=$APPHASH/g" Dockerfile.cc
sed -i "s/ARG BRANCH_NAME/ENV BUILD_BRANCH=$APPHASH/g" Dockerfile.arm
git add Dockerfile
git commit -m"beta(): bump to 3.$APPVERSION-beta"
git push -u origin 3.$APPVERSION-beta
fi;
