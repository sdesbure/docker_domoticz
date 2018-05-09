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
	libsqlite3-0 libsqlite3-dev:armhf \
	curl libcurl3 libcurl4-openssl-dev:armhf \
	libusb-0.1-4 libusb-dev:armhf \
	zlib1g-dev:armhf \
	libudev-dev libudev-dev:armhf \
	libpython3-dev:armhf python3-pip \
      && \
      cp /usr/share/zoneinfo/Europe/Paris /etc/localtime 

ENV CC arm-linux-gnueabihf-gcc
ENV CXX arm-linux-gnueabihf-g++
ENV LD arm-linux-gnueabihf-g++
ENV CROSS_COMPILE arm-linux-gnueabihf-

RUN apt-get install -yq wget && mkdir boost &&\
				cd boost &&\
				wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz &&\
				tar xfz boost_1_66_0.tar.gz &&\
				rm boost_1_66_0.tar.gz &&\
				cd boost_1_66_0/ &&\
    ./bootstrap.sh &&\
    sed -i 's/using gcc ;/using gcc : arm : arm-linux-gnueabihf-g++ ;/' project-config.jam &&\
    ./bjam install toolset=gcc-arm --prefix=/usr/local/boost  threading=multi link=static --with-thread --with-date_time --with-system --with-atomic --with-regex  &&\
				cd ../../ &&\
				rm -Rf boost/

ENV BOOST_ROOT /usr/local/boost/

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

FROM resin/armv7hf-debian-qemu
COPY --from=0 /src /src


LABEL org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url="https://github.com/domoticz/domoticz" \
      org.label-schema.url="https://domoticz.com/" \
      org.label-schema.name="Domoticz" \
      org.label-schema.docker.dockerfile="/Dockerfile" \
      org.label-schema.license="GPLv3" \
      org.label-schema.build-date=$BUILD_DATE

RUN ["cross-build-start"]

RUN \
  apt-get update && \
  apt-get install -y cmake apt-utils build-essential && \
  apt-get install -y libboost-dev libboost-thread-dev libboost-system-dev libsqlite3-dev subversion curl libcurl4-openssl-dev libusb-dev zlib1g-dev libpython3-dev && \
  apt-get install -y iputils-ping && \
  apt-get clean && \
  apt-get autoclean && \
  rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN ["cross-build-end"]

VOLUME /config

EXPOSE 8080

ENTRYPOINT ["/src/domoticz/domoticz", "-dbase", "/config/domoticz.db", "-log", "/config/domoticz.log"]
CMD ["-www", "8080"]
