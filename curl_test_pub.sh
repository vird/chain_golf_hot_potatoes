#!/bin/bash
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_node_list"}' -H 'content-type:text/plain;' http://localhost:10002
