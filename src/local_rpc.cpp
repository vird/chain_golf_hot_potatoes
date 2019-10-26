class LocalServer : public AbstractServer<LocalServer> {
 public:
  bool work = true;
  LocalServer(AbstractServerConnector &conn, serverVersion_t type = JSONRPC_SERVER_V2) : AbstractServer<LocalServer>(conn, type) {
    // common
    bindAndAddMethod(Procedure(
      "bc_height", PARAMS_BY_NAME, JSON_INTEGER,
        NULL),
      &LocalServer::bc_heightI);
    bindAndAddMethod(Procedure(
      "get_node_list", PARAMS_BY_NAME, JSON_ARRAY,
        NULL),
      &LocalServer::get_node_listI);
    
    // prv only
    bindAndAddMethod(Procedure(
      "get_balance", PARAMS_BY_NAME, JSON_INTEGER,
         "address", JSON_INTEGER,
           NULL),
      &LocalServer::balanceI);
    bindAndAddMethod(Procedure(
      "shutdown", PARAMS_BY_NAME, JSON_STRING,
           NULL),
      &LocalServer::shutdownI);
    bindAndAddMethod(Procedure(
      "transfer", PARAMS_BY_NAME, JSON_STRING,
        "amount", JSON_INTEGER,
        "from_address", JSON_INTEGER,
        "to_address", JSON_INTEGER,
        NULL),
      &LocalServer::transferI);
    bindAndAddMethod(Procedure(
      "address_transfer", PARAMS_BY_NAME, JSON_STRING,
        "address", JSON_INTEGER,
        "pub_key", JSON_STRING,
        NULL),
      &LocalServer::address_transferI);
    // good for debug
    bindAndAddMethod(Procedure(
      "debug_set_key", PARAMS_BY_NAME, JSON_STRING,
        "address", JSON_INTEGER,
        "pub_key", JSON_STRING,
        "prv_key", JSON_STRING,
        NULL),
      &LocalServer::debug_set_keyI);
    bindAndAddMethod(Procedure(
      "debug_key_gen", PARAMS_BY_NAME, JSON_STRING,
        NULL),
      &LocalServer::debug_key_genI);
  }
  
  void bc_heightI(const Value &request, Value &response) {
    bc_height(request, response);
  }
  
  void get_node_listI(const Value &request, Value &response) {
    get_node_list(request, response);
  }
  
  void balanceI(const Value &request, Value &response) {
    // const char* addr = request["address"].asString().c_str();
    // u32 address = atoi(addr);
    u32 address = request["address"].asInt();
    if (address >= gms.balance.size()) {
      response = 0;
      return;
      // throw JsonRpcException(-1, "Address not exists");
    }
    response = gms.balance[address];
  }
  
  void shutdownI(const Value &request, Value &response) {
    printf("shutdown scheduled\n");
    work = false;
    response = "ok";
  }
  
  void transferI(const Value &request, Value &response) {
    u32 amount = request["amount"].asInt();
    
    u32 from_address = request["from_address"].asInt();
    if (from_address >= gms.balance.size()) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "from_address not exists");
    }
    u32 to_address = request["to_address"].asInt();
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
    u32 address = request["address"].asInt();
    if (gms.a2pk[address] != my_pub_key) {
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
  
  void debug_set_keyI(const Value &request, Value &response) {
    u32 address = request["address"].asInt();
    if (address >= gms.balance.size()) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "Address not exists");
    }
    
    // TODO move to define
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
    
    string hex_prv_key = request["prv_key"].asString();
    if (hex_prv_key.size() != 2*t_prv_key_size) {
      response = "fail";
      return;
      // throw JsonRpcException(-1, "bad prv_key size");
    }
    for(int i=0;i<2*t_prv_key_size;i++) {
      char ch = hex_prv_key[i];
      if ('0' <= ch && ch <= '9') continue;
      if ('a' <= ch && ch <= 'f') continue;
      if ('A' <= ch && ch <= 'A') continue;
      response = "fail";
      return;
      // throw JsonRpcException(-1, "bad prv_key hex");
    }
    
    t_pub_key pub_key;
    t_prv_key prv_key;
    
    str2t_pub_key(hex_pub_key, pub_key);
    str2t_prv_key(hex_prv_key, prv_key);
    
    if (gms.a2pk[address] != pub_key) {
      t_pub_key_print("pub_key           = ", pub_key);
      t_pub_key_print("gms.a2pk[address] = ", gms.a2pk[address]);
      response = "fail";
      return;
      // throw JsonRpcException(-2, "address and pub_key mismatch");
    }
    
    my_primary_address = address;
    my_pub_key = pub_key;
    my_prv_key = prv_key;
    t_pub_key_print("my_pub_key           = ", my_pub_key);
    t_prv_key_print("my_prv_key           = ", my_prv_key);
    
    response = "ok";
  }
  
  void debug_key_genI(const Value &request, Value &response) {
    unsigned char seed[32];
    if (ed25519_create_seed(seed)) {
      response = "fail";
      return;
      // throw JsonRpcException(-2, "error while generating seed");
    }
    t_pub_key pub_key;
    t_prv_key prv_key;
    ed25519_create_keypair(pub_key.b, prv_key.b, seed);
    
    t_pub_key_print("pub_key = ", pub_key);
    t_prv_key_print("prv_key = ", prv_key);
    
    response = "ok";
  }
};
