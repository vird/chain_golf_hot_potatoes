void rpc_bc_height(const Value &request, Value &response) {
  response = bc_height();
}
void rpc_get_node_list(const Value &request, Value &response) {
  FOR_COL(it, gns.node_list) {
    Value node;
    node["is_self"] = it->is_self;
    node["ip_port"] = it->ip_port;
    response.append(node);
  }
}
void rpc_get_block_number(const Value &request, Value &response) {
  // не совсем конкретно, т.к. есть offset
  // если вылетаем за пределы offset'а снизу, то надо читать с базы...
  i64 id = request["id"].asInt();
  if (id < 0) {
    response = "fail";
    return;
    // throw JsonRpcException(-1, "id < 0");
  }
  if (id >= gms.main_chain_block_list.size()) {
    response = "fail";
    return;
    // throw JsonRpcException(-1, "id not exists");
  }
  block_to_json(gms.main_chain_block_list[id], response);
}

void rpc_tx_push(const Value &request, Value &response) {
  Tx tx;
  tx.type     = request["type"].asInt();
  tx.amount   = request["amount"].asInt();
  if (!address_json_parse(request["send_addr"], tx.send_addr)) {
    response = "fail";
    return;
    // throw JsonRpcException(-1, "invalid send_addr");
  }
  if (!address_json_parse(request["recv_addr"], tx.recv_addr)) {
    response = "fail";
    return;
    // throw JsonRpcException(-1, "invalid recv_addr");
  }
  if (!str2t_pub_key(request["bind_pub_key"].asString(), tx.bind_pub_key)) {
    response = "fail";
    return;
    // throw JsonRpcException(-1, "bad bind_pub_key");
  }
  // will be later
  // tx.tx_epoch = request["tx_epoch"].asInt();
  tx.nonce    = request["nonce"].asInt();
  
  if (!str2t_hash(request["hash"].asString(), tx.hash)) {
    response = "fail";
    return;
    // throw JsonRpcException(-1, "bad hash");
  }
  if (!str2t_sign(request["sign"].asString(), tx.sign)) {
    response = "fail";
    return;
    // throw JsonRpcException(-1, "bad sign");
  }
  
  // pre-validation
  if (!tx_validate(tx)) {
    response = "fail";
    return;
    // throw JsonRpcException(-1, "validation fail");
  }
  
  gms.tx_list.push_back(tx);
}