#!/bin/bash
curl --silent https://releases.domoticz.com/releases/beta/version_linux_x86_64.h | awk '{print $2"="$3}' > beta.env
source beta.env
rm beta.env
git branch -r | grep "4.$APPVERSION-beta"
EXIST=$?
if [[ $EXIST != "0" ]]; then
git checkout -b 4.$APPVERSION-beta
sed -i "s/ARG APP_HASH/ENV APP_HASH=$APPHASH/g" Dockerfile
sed -i "s/ARG BRANCH_NAME/ENV BUILD_BRANCH=$APPHASH/g" Dockerfile.cc
sed -i "s/ARG BRANCH_NAME/ENV BUILD_BRANCH=$APPHASH/g" Dockerfile.arm
sed -i "s/ARG BRANCH_NAME/ENV BUILD_BRANCH=$APPHASH/g" Dockerfile.deb
git add Dockerfile
git commit -m"beta(): bump to 4.$APPVERSION-beta"
git push -u origin 4.$APPVERSION-beta
fi;
