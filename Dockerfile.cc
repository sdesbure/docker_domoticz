FROM debian:stretch
MAINTAINER Cedric Gatay <c.gatay@code-troopers.com>

# install packages &
## OpenZwave installation &
# grep git version of openzwave &
# untar the files &
# compile &
# "install" in order to be found by domoticz &
## Domoticz installation &
# clone git source in src &
# Domoticz needs the full history to be able to calculate the version string &
# prepare makefile &
# compile &
# remove git and tmp dirs

ARG VCS_REF
ARG BUILD_DATE

ARG BRANCH_NAME

LABEL org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url="https://github.com/domoticz/domoticz" \
      org.label-schema.url="https://domoticz.com/" \
      org.label-schema.name="Domoticz" \
      org.label-schema.docker.dockerfile="/Dockerfile" \
      org.label-schema.license="GPLv3" \
      org.label-schema.build-date=$BUILD_DATE


# install packages
RUN dpkg --add-architecture armhf && apt-get update && apt-get install -yq \
	git gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf \
	libssl1.0.2 libssl-dev:armhf \
	cmake \
      libboost-all-dev:armhf libboost-graph-parallel-dev:armhf libboost-graph-parallel1.62-dev:armhf libboost-graph-parallel1.62.0:armhf \
      libboost-mpi-dev:armhf libboost-mpi1.62.0:armhf libboost-mpi-python-dev:armhf libboost-mpi-python1.62.0:armhf\
	libsqlite3-0 libsqlite3-dev:armhf \
	curl libcurl3 libcurl4-openssl-dev:armhf \
	libusb-0.1-4 libusb-dev:armhf \
	zlib1g-dev:armhf \
	libudev-dev libudev-dev:armhf \
	python3-dev:armhf python3-pip \
      fail2ban && \
      cp /usr/share/zoneinfo/Europe/Paris /etc/localtime 


ENV CC arm-linux-gnueabihf-gcc
ENV CXX arm-linux-gnueabihf-g++
ENV LD arm-linux-gnueabihf-g++
ENV CROSS_COMPILE arm-linux-gnueabihf-

RUN   git clone --depth 2 https://github.com/OpenZWave/open-zwave.git /src/open-zwave && \
      cd /src/open-zwave && \
      make && \
      ln -s /src/open-zwave /src/open-zwave-read-only && \
      git clone -b ${BRANCH_NAME:-development} --depth 2 https://github.com/domoticz/domoticz.git /src/domoticz && \
      cd /src/domoticz && \
      git fetch --unshallow && \
      cmake -DCMAKE_BUILD_TYPE=Release . && \
      make && \
      rm -rf /src/domoticz/.git && \
      rm -rf /src/open-zwave/.git && \
      apt-get remove -y git cmake linux-headers-amd64 build-essential libssl-dev libboost-dev libboost-thread-dev libboost-system-dev libsqlite3-dev libcurl4-openssl-dev libusb-dev zlib1g-dev libudev-dev && \
      apt-get autoremove -y && \ 
      apt-get clean && \
      rm -rf /var/lib/apt/lists/*


FROM schmich/armv7hf-alpine-qemu:3.5
COPY --from=0 /src /src


LABEL org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url="https://github.com/domoticz/domoticz" \
      org.label-schema.url="https://domoticz.com/" \
      org.label-schema.name="Domoticz" \
      org.label-schema.docker.dockerfile="/Dockerfile" \
      org.label-schema.license="GPLv3" \
      org.label-schema.build-date=$BUILD_DATE

RUN ["cross-build-start"]

RUN apk add --no-cache \
	 libssl1.0 \
       tzdata \
	 boost-thread \
	 boost-system \
	 boost-date_time \
	 sqlite \
	 curl libcurl \
	 libusb \
	 zlib \
	 udev \
	 python3-dev && \
	 cp /usr/share/zoneinfo/Europe/Paris /etc/localtime && \
       apk del tzdata

RUN ["cross-build-end"]

VOLUME /config

EXPOSE 8080

ENTRYPOINT ["/src/domoticz/domoticz", "-dbase", "/config/domoticz.db", "-log", "/config/domoticz.log"]
CMD ["-www", "8080"]