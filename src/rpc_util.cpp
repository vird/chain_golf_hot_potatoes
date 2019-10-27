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
