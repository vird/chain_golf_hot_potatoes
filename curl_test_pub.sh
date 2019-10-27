#!/bin/bash
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"bc_height"}' -H 'content-type:text/plain;' http://localhost:10002
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_node_list"}' -H 'content-type:text/plain;' http://localhost:10002
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"handshake", "params": {"rev_ip_port" : "127.0.0.1:1000"}}' -H 'content-type:text/plain;' http://localhost:10002
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_node_list"}' -H 'content-type:text/plain;' http://localhost:10002

curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_block_number", "params": {"id": 0}}' -H 'content-type:text/plain;' http://localhost:10002
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_block_number", "params": {"id": 1}}' -H 'content-type:text/plain;' http://localhost:10002
curl --data-binary '{"id":1,"jsonrpc":"2.0","method":"get_block_number", "params": {"id": 2}}' -H 'content-type:text/plain;' http://localhost:10002
