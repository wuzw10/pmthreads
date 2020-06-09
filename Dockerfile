FROM ubuntu:18.04
MAINTAINER Zhenwei Wu wuzw16@gmail.com
ENV TZ=Europe/Minsk
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN dpkg --add-architecture i386
RUN apt-get update && apt-get install -y \
	git build-essential cmake libboost-dev python numactl wget gcc-multilib libevent-dev scons libconfig++-dev libelf-dev libnuma-dev libssl-dev  libz-dev libjpeg-dev libgsl-dev automake libboost-all-dev \    
build-essential gcc curl  make vim libpcre3-dev gcc-multilib g++-multilib libc6-dev-i386 libjpeg62:i386 python-numpy python-setuptools python-pip python-matplotlib 
COPY ./pmthreads /root/pmthreads