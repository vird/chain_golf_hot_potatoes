#!/usr/bin/env bash
jsonrpcstub prv_spec.json \
  --cpp-server=AbstractStubServer \
  --cpp-client=StubClient \
  --cpp-server-file=prv_server.h \
  --cpp-client-file=prv_client.h

jsonrpcstub pub_spec.json \
  --cpp-server=AbstractStubServer \
  --cpp-client=StubClient \
  --cpp-server-file=pub_server.h \
  --cpp-client-file=pub_client.h
