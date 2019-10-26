#!/bin/bash
# curl --data-binary '{"method": "get_balance", "params": {"address":"address"}, "id": 1}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":0}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":1}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":2}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":100000002}}' -H 'content-type:text/plain;' http://localhost:10001