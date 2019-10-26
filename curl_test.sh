#!/bin/bash
# curl --data-binary '{"method": "get_balance", "params": {"address":"address"}, "id": 1}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":0}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":1}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":2}}' -H 'content-type:text/plain;' http://localhost:10001
# fail
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":100000002}}' -H 'content-type:text/plain;' http://localhost:10001

curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":0, "to_address": 1, "amount":100}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":0}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":1}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":2}}' -H 'content-type:text/plain;' http://localhost:10001

curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":0, "to_address": 100000002, "amount":100}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":100000002, "to_address": 1, "amount":100}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":0, "to_address": 1, "amount":-1}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":0, "to_address": 1, "amount":100000002}}' -H 'content-type:text/plain;' http://localhost:10001
# TODO test not own

# curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"shutdown"}' -H 'content-type:text/plain;' http://localhost:10001

