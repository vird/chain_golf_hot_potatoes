#!/usr/bin/env bash

IMAGE_NAME="node"
CLIENT_NAME="client"

g++ test_client.cpp -ljsoncpp -lcurl -ljsonrpccpp-common -ljsonrpccpp-client -o ${CLIENT_NAME} || exit 1

docker build . -t ${IMAGE_NAME}

docker run -p 10001:10001 ${IMAGE_NAME} -d &

sleep 15

./${CLIENT_NAME}

docker stop $(docker ps -a -q)
docker rm $(docker ps -a -q)
