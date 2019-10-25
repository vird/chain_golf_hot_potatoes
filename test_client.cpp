#include <iostream>

#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>

class Client : public jsonrpc::Client {
 public:
  Client(jsonrpc::IClientConnector &conn,
         jsonrpc::clientVersion_t type = jsonrpc::JSONRPC_CLIENT_V2)
      : jsonrpc::Client(conn, type) {}

  int balance(const std::string &address) {
    Json::Value p;
    p["address"] = address;
    Json::Value result = CallMethod("balance", p);
    if (result.isIntegral())
      return result.asInt();
    else
      throw jsonrpc::JsonRpcException(
          jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
          result.toStyledString());
  }
  std::string transaction(int amount, const std::string &from_address,
                          const std::string &to_address) {
    Json::Value p;
    p["amount"] = amount;
    p["from_address"] = from_address;
    p["to_address"] = to_address;
    Json::Value result = CallMethod("transaction", p);
    if (result.isString())
      return result.asString();
    else
      throw jsonrpc::JsonRpcException(
          jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
          result.toStyledString());
  }
};

int main() {
  jsonrpc::HttpClient httpclient("http://localhost:10001");
  Client c(httpclient, jsonrpc::JSONRPC_CLIENT_V2);

  try {
    std::cout << "CLIENT | Received by balance: " << c.balance("address")
              << std::endl;
    std::cout << "CLIENT | Received by transaction: "
              << c.transaction(1, "from_address", "to_address") << std::endl;
  } catch (jsonrpc::JsonRpcException &e) {
    std::cerr << e.what() << std::endl;
  }
}