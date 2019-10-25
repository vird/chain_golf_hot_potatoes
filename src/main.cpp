#define u64 unsigned long long
#define i64 long long
#define u32 unsigned int
#define i32 int
#define u8 unsigned char
#define F(i) r |= s[i]
#include<cstdio>
#include<filesystem>
#include<iostream>
#include<thread>
#include<jsonrpccpp/server.h>
#include<jsonrpccpp/server/connectors/httpserver.h>
#include<stdint.h>

#include "ed25519.h"
#include "fe.c"
#include "ge.c"
#include "sc.c"

using namespace std;
using namespace jsonrpc;
using namespace Json;

class TemplateServer : public AbstractServer<TemplateServer> {
 public:
  TemplateServer(AbstractServerConnector &conn,
                 serverVersion_t type = JSONRPC_SERVER_V2)
      : AbstractServer<TemplateServer>(conn, type) {
    bindAndAddMethod(
        Procedure("balance", PARAMS_BY_NAME,
                           JSON_INTEGER, "address",
                           JSON_STRING, NULL),
        &TemplateServer::balanceI);
    bindAndAddMethod(
        Procedure(
            "transaction", PARAMS_BY_NAME, JSON_STRING,
            "amount", JSON_INTEGER, "from_address",
            JSON_STRING, "to_address", JSON_STRING, NULL),
        &TemplateServer::transactionI);
  }

  virtual void balanceI(const Value &request,
                               Value &response) {
    response = balance(request["address"].asString());
  }
  virtual void transactionI(const Value &request,
                            Value &response) {
    response = transaction( request["amount"].asInt(),
                            request["from_address"].asString(),
                            request["to_address"].asString());
  }
  virtual int balance(const string &address) = 0;
  virtual string transaction(int amount, const string &from_address,
                                  const string &to_address) = 0;
};

class MyServer : public TemplateServer {
 public:
  MyServer(AbstractServerConnector &connector,
               serverVersion_t type);

  int balance(const string &address) override;
  string transaction( int amount,
                      const string &from_address,
                      const string &to_address) override;
};

MyServer::MyServer(AbstractServerConnector &connector,
                           serverVersion_t type)
    : TemplateServer(connector, type) {}

int MyServer::balance(const string &address) {
  cout << "SERVER | Received in balance: address[" << address << "]"
            << endl;
  return 1;
}

string MyServer::transaction(int amount,
                            const string &from_address,
                            const string &to_address) {
  cout << "SERVER | Received in transaction: from_address[" << from_address
            << "], to_address[" << to_address << "], amount[" << amount << "]"
            << endl;
  return "transaction block hash";
}

int main() {
  HttpServer httpserver(10001);
  MyServer s(httpserver, JSONRPC_SERVER_V1V2);
  s.StartListening();
  cout << "welcome to UTON HACK!" << endl;
  this_thread::sleep_for(chrono::seconds(45));
  s.StopListening();
  return 0;
}