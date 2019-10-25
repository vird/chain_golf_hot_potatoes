FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    apt-get update -y && \
    apt-get install -y apt-utils && \
    apt-get dist-upgrade -y

RUN add-apt-repository http://security.ubuntu.com/ubuntu/ && \
    add-apt-repository ppa:jonathonf/gcc-9.0 && \
    apt-get update -y && \
    apt-get dist-upgrade -y

RUN apt-get install -y gcc-9 g++-9 make libicu-dev libbz2-dev wget

RUN apt-get install -y libssl-dev libcurl4 libcurl4-openssl-dev

RUN BOOST_GENERATION=1 && \
    BOOST_MAJOR_VERSION=71 && \
    BOOST_MINOR_VERSION=0 && \
    wget -O boost_${BOOST_GENERATION}_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}.tar.gz \
    https://dl.bintray.com/boostorg/release/${BOOST_GENERATION}.${BOOST_MAJOR_VERSION}.${BOOST_MINOR_VERSION}/source/boost_${BOOST_GENERATION}_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}.tar.gz && \
    tar xzvf boost_${BOOST_GENERATION}_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}.tar.gz && \
    cd boost_${BOOST_GENERATION}_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}/ && \
    ./bootstrap.sh --prefix=/usr/ && \
    ./b2 && \
    ./b2 install

RUN apt-get install -y libjsonrpccpp-dev libjsonrpccpp-tools

WORKDIR /project

COPY . /project

RUN cd /project && \
    chmod +x /project/build.sh && \
    /project/build.sh && \
    chmod +x /project/run.sh

ENTRYPOINT /project/run.sh
