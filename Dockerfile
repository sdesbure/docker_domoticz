FROM alpine:3.7
MAINTAINER Sylvain Desbureaux <sylvain@desbureaux.fr> #Original creator of this Dockerfile
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

ARG APP_HASH
ARG VCS_REF
ARG BUILD_DATE

LABEL org.label-schema.vcs-ref=$APP_HASH \
      org.label-schema.vcs-url="https://github.com/domoticz/domoticz" \
      org.label-schema.url="https://domoticz.com/" \
      org.label-schema.name="Domoticz" \
      org.label-schema.docker.dockerfile="/Dockerfile" \
      org.label-schema.license="GPLv3" \
      org.label-schema.build-date=$BUILD_DATE

RUN apk add --no-cache \
		git \
		python3-dev \
		build-base cmake \
		boost-dev \
		boost-thread \
		boost-system \
		boost-date_time \
		sqlite sqlite-dev \
		curl libcurl curl-dev \
		libssl1.0 libressl-dev \
		libusb libusb-dev \
		libusb-compat libusb-compat-dev \
		lua5.2-dev \
		minizip-dev \
		mosquitto-dev \
		coreutils \
		tzdata \
		zlib zlib-dev \
		udev eudev-dev \
		linux-headers && \
	# Build OpenZwave
	git clone --depth 2 https://github.com/OpenZWave/open-zwave.git /src/open-zwave && \
	ln -s /src/open-zwave /src/open-zwave-read-only && \
	cd /src/open-zwave && \
	make && \
 	make \
		instlibdir=usr/lib \
		pkgconfigdir="usr/lib/pkgconfig/" \
		PREFIX=/usr \
		sysconfdir=etc/openzwave \
	install && \
	cd / && \
	rm -rf /src/open-zwave && \
	# Build Domoticz
	git clone https://github.com/domoticz/domoticz.git /src/domoticz && \
	cd /src/domoticz && \
	git reset --hard ${APP_HASH} && \
	cmake \
	 	-DBUILD_SHARED_LIBS=True \
	 	-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=/opt/domoticz \
		-DOpenZWave=/usr/lib/libopenzwave.so \
		-DUSE_BUILTIN_LUA=OFF \
		-DUSE_BUILTIN_MINIZIP=OFF \
		-DUSE_BUILTIN_MQTT=OFF \
		-DUSE_BUILTIN_SQLITE=OFF \
		-DUSE_STATIC_OPENZWAVE=OFF \
		-Wno-dev && \
	make && \
	make install && \
	rm -rf /src/domoticz/ && \
	# Cleanup
	apk del \ 
		git \
		build-base cmake \
		boost-dev \
		sqlite-dev \
		curl-dev \
		libressl-dev \
		libusb-dev \
		libusb-compat-dev \
		coreutils \
		zlib-dev \
		eudev-dev \
		linux-headers

VOLUME /config

EXPOSE 8080

ENTRYPOINT ["/opt/domoticz/domoticz", "-dbase", "/config/domoticz.db", "-log", "/config/domoticz.log"]
CMD ["-www", "8080"]
