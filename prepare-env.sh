#!/usr/bin/env bash

# common updates
apt-get update -y
apt-get install -y software-properties-common
apt-get update -y
apt-get install -y apt-utils
apt-get dist-upgrade -y
add-apt-repository -y http://security.ubuntu.com/ubuntu/
add-apt-repository -y ppa:jonathonf/gcc-9.0
apt-get update -y
apt-get dist-upgrade -y

# install development dependencies
apt-get install -y gcc-9 g++-9 make libicu-dev libbz2-dev wget libssl-dev libcurl4 libcurl4-openssl-dev

BOOST_GENERATION=1
BOOST_MAJOR_VERSION=71
BOOST_MINOR_VERSION=0

wget -O boost_${BOOST_GENERATION}_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}.tar.gz \
  https://dl.bintray.com/boostorg/release/${BOOST_GENERATION}.${BOOST_MAJOR_VERSION}.${BOOST_MINOR_VERSION}/source/boost_${BOOST_GENERATION}_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}.tar.gz

tar xzvf boost_${BOOST_GENERATION}_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}.tar.gz

cd boost_${BOOST_GENERATION}_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}/ || exit 1

./bootstrap.sh --prefix=/usr/

./b2

./b2 install

apt-get install -y libjsonrpccpp-dev libjsonrpccpp-tools

# install other dependencies
apt-get install clang

apt-get install docker-engine

apt-get install iproute2
