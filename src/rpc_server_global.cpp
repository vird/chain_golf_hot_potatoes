class GlobalServer : public AbstractServer<GlobalServer> {
 public:
  bool work = true;
  GlobalServer(AbstractServerConnector &conn, serverVersion_t type = JSONRPC_SERVER_V2) : AbstractServer<GlobalServer>(conn, type) {
    // common
    bindAndAddMethod(Procedure(
      "bc_height", PARAMS_BY_NAME, JSON_INTEGER,
        NULL),
      &GlobalServer::bc_heightI);
    bindAndAddMethod(Procedure(
      "get_node_list", PARAMS_BY_NAME, JSON_ARRAY,
        NULL),
      &GlobalServer::get_node_listI);
    bindAndAddMethod(Procedure(
      "get_block_number", PARAMS_BY_NAME, JSON_OBJECT,
        NULL),
      &GlobalServer::get_block_numberI);
    // pub only
    bindAndAddMethod(Procedure(
      "handshake", PARAMS_BY_NAME, JSON_STRING,
        "rev_ip_port", JSON_STRING,
          NULL),
      &GlobalServer::handshakeI);
    // БОЛЬ
    
    bindAndAddMethod(Procedure(
      "get_balance", PARAMS_BY_NAME, JSON_INTEGER,
         "address", JSON_STRING,
           NULL),
      &GlobalServer::balanceI);
    bindAndAddMethod(Procedure(
      "transfer", PARAMS_BY_NAME, JSON_STRING,
        "amount", JSON_INTEGER,
        "from_address", JSON_STRING,
        "to_address", JSON_STRING,
        NULL),
      &GlobalServer::transferI);
    bindAndAddMethod(Procedure(
      "address_transfer", PARAMS_BY_NAME, JSON_STRING,
        "address", JSON_STRING,
        "pub_key", JSON_STRING,
        NULL),
      &GlobalServer::address_transferI);
  }
  
  void bc_heightI(const Value &request, Value &response) {
    rpc_bc_height(request, response);
  }
  
  void get_node_listI(const Value &request, Value &response) {
    rpc_get_node_list(request, response);
  }
  
  void get_block_numberI(const Value &request, Value &response) {
    rpc_get_block_number(request, response);
  }
  
  // NOTE can exhaust memory.
  // LATER add protection
  void handshakeI(const Value &request, Value &response) {
    string rev_ip_port = request["rev_ip_port"].asString();
    // https://stackoverflow.com/questions/1076714/max-length-for-client-ip-address/1076749
    // ipv6 = 45
    // https://stackoverflow.com/questions/186829/how-do-ports-work-with-ipv6
    // + [] 2
    // + port 6
    // 53
    // we reserve up to
    if (rev_ip_port.size() > 100) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "bad rev_ip_port");
    }
    // TODO verify format better
    // TODO normalize
    auto end = gns.node_list.end();
    bool found = false;
    FOR_COL(it, gns.node_list) {
      if (it->ip_port == rev_ip_port) {
        found = true;
        break;
      }
    }
    if (!found) {
      NetNode node;
      node.ip_port = rev_ip_port;
      gns.node_list.push_back(node);
    }
    
    response = "ok";
  }
  
  // БОЛЬ
  
  void balanceI(const Value &request, Value &response) {
    // const char* addr = request["address"].asString().c_str();
    // u32 address = atoi(addr);
    u32 address;
    if (!address_json_parse(request["address"], address)) {
      response = 0;
      return;
      // throw JsonRpcException(-1, "Invalid address");
    }
    if (address >= gms.balance.size()) {
      response = 0;
      return;
      // throw JsonRpcException(-1, "Address not exists");
    }
    response = gms.balance[address];
  }
  void transferI(const Value &request, Value &response) {
    u32 amount = request["amount"].asInt();
    
    u32 from_address;
    if (!address_json_parse(request["from_address"], from_address)) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "Invalid from_address");
    }
    if (from_address >= gms.balance.size()) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "from_address not exists");
    }
    
    u32 to_address;
    if (!address_json_parse(request["to_address"], to_address)) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "Invalid to_address");
    }
    if (to_address >= gms.balance.size()) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "to_address not exists");
    }
    if (gms.a2pk[from_address] != my_pub_key) {
      response = "fail";
      return;
      // throw JsonRpcException(-2, "you don't own from_address");
    }
    // overflow protection
    if (gms.balance[from_address] < max(amount, amount + tx_fee)) {
      response = "fail";
      return;
      // throw JsonRpcException(-3, "not enough balance");
    }
    
    Tx tx;
    tx.type     = 1;
    tx.amount   = amount;
    tx.send_addr= from_address;
    tx.recv_addr= to_address;
    tx.nonce    = 0;
    tx_sign(tx, my_pub_key, my_prv_key);
    
    if (!tx_validate(tx)) {
      printf("tx_validate_reason = %d\n", tx_validate_reason);
      response = "fail";
      return;
      // throw JsonRpcException(-9, "tx_validate fail");
    }
    
    printf("tx transfer %d coin %d -> %d\n", amount, from_address, to_address);
    
    gms.tx_list.push_back(tx);
    
    response = "ok";
  }
  
  void address_transferI(const Value &request, Value &response) {
    u32 address;
    if (!address_json_parse(request["address"], address)) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "Invalid address");
    }
    if (address >= gms.a2pk.size()) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "to_address not exists");
    }
    if (gms.a2pk[address] != my_pub_key) {
      printf("you don't own address %d\n", address);
      t_pub_key_print("my_pub_key        = ", my_pub_key);
      t_pub_key_print("gms.a2pk[address] = ", gms.a2pk[address]);
      response = "fail";
      return;
      // throw JsonRpcException(-2, "you don't own address");
    }
    string hex_pub_key = request["pub_key"].asString();
    if (hex_pub_key.size() != 2*t_pub_key_size) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "bad pub_key size");
    }
    for(int i=0;i<2*t_pub_key_size;i++) {
      char ch = hex_pub_key[i];
      if ('0' <= ch && ch <= '9') continue;
      if ('a' <= ch && ch <= 'f') continue;
      if ('A' <= ch && ch <= 'A') continue;
      response = "fail";
      return;
      // throw JsonRpcException(-1, "bad pub_key hex");
    }
    
    Tx tx;
    tx.type     = 2;
    tx.amount   = 0;
    tx.send_addr= my_primary_address;
    tx.recv_addr= address;
    
    str2t_pub_key(hex_pub_key, tx.bind_pub_key);
    tx.nonce    = 0;
    tx_sign(tx, my_pub_key, my_prv_key);
    
    if (!tx_validate(tx)) {
      printf("tx_validate_reason = %d\n", tx_validate_reason);
      response = "fail";
      return;
      // throw JsonRpcException(-9, "tx_validate fail");
    }
    
    printf("address transfer owner=%d address=%d pub_key=%s\n", my_primary_address, address, hex_pub_key.c_str());
    
    gms.tx_list.push_back(tx);
    
    response = "ok";
  }
};
