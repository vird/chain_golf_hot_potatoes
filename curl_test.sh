#!/bin/bash
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"debug_key_gen"}' -H 'content-type:text/plain;' http://localhost:10001
# pub_key=d67b8af90684a7220be81fb3fa4a3477a4a8abf56dad3856f1a4687868b34343
# prv_key=e89ecc5ea4e671941fe8b904b30b334793ff7d81f0f7bb44efb48f4eb2d47651906e8d78d7bcb26cb6f553556273ee9e6f9d812d4d697a7453c03fe74df1043f

# curl --data-binary '{"method": "get_balance", "params": {"address":"address"}, "id": 1}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":0}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":1}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":2}}' -H 'content-type:text/plain;' http://localhost:10001
# fail
echo -n "FAIL " && curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":100000002}}' -H 'content-type:text/plain;' http://localhost:10001

curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":0, "to_address": 1, "amount":100}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":0}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":1}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":2}}' -H 'content-type:text/plain;' http://localhost:10001
# fail
echo -n "FAIL " && curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":0, "to_address": 100000002, "amount":100}}' -H 'content-type:text/plain;' http://localhost:10001
echo -n "FAIL " && curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":100000002, "to_address": 1, "amount":100}}' -H 'content-type:text/plain;' http://localhost:10001
echo -n "FAIL " && curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":0, "to_address": 1, "amount":-1}}' -H 'content-type:text/plain;' http://localhost:10001
echo -n "FAIL " && curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":0, "to_address": 1, "amount":100000002}}' -H 'content-type:text/plain;' http://localhost:10001
# TODO test not own
# curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"address_transfer","params":{"address":1, "pub_key": "d67b8af90684a7220be81fb3fa4a3477a4a8abf56dad3856f1a4687868b34343"}}' -H 'content-type:text/plain;' http://localhost:10001
# fail
# echo -n "FAIL " && curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"address_transfer","params":{"address":1, "pub_key": "d67b8af90684a7220be81fb3fa4a3477a4a8abf56dad3856f1a4687868b34343"}}' -H 'content-type:text/plain;' http://localhost:10001
echo -n "FAIL " && curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"address_transfer","params":{"address":1, "pub_key": "d67b8af90684a7220be81fb3fa4a3477a4a8abf56dad3856f1a4687868b34343"}}' -H 'content-type:text/plain;' http://localhost:10001

echo "try another key pair"
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"debug_set_key","params":{"address": 1, "pub_key":"d67b8af90684a7220be81fb3fa4a3477a4a8abf56dad3856f1a4687868b34343", "prv_key": "e89ecc5ea4e671941fe8b904b30b334793ff7d81f0f7bb44efb48f4eb2d47651906e8d78d7bcb26cb6f553556273ee9e6f9d812d4d697a7453c03fe74df1043f"}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"transfer","params":{"from_address":1, "to_address": 0, "amount":99}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":0}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":1}}' -H 'content-type:text/plain;' http://localhost:10001
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_balance","params":{"address":2}}' -H 'content-type:text/plain;' http://localhost:10001
# fail
echo -n "FAIL " && curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"debug_set_key","params":{"address": 0, "pub_key":"d67b8af90684a7220be81fb3fa4a3477a4a8abf56dad3856f1a4687868b34343", "prv_key": "e89ecc5ea4e671941fe8b904b30b334793ff7d81f0f7bb44efb48f4eb2d47651906e8d78d7bcb26cb6f553556273ee9e6f9d812d4d697a7453c03fe74df1043f"}}' -H 'content-type:text/plain;' http://localhost:10001

# curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"shutdown"}' -H 'content-type:text/plain;' http://localhost:10001

