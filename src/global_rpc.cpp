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
    auto it = find(gns.node_list.begin(), end, rev_ip_port);
    if (it == end) {
      gns.node_list.push_back(rev_ip_port);
    }
    
    response = "ok";
  }
};
