#include <cstdio>
#include <filesystem>
#include <iostream>
#include <thread>

#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>

using namespace std;
using namespace jsonrpc;
using namespace Json;

// type related
#define I int
#define S string
#define C const
// server related
#define TS TemplateServer
#define ASC AbstractServerConnector
#define AS AbstractServer
#define sVt serverVersion_t
// JSON
#define PBN PARAMS_BY_NAME
#define JI JSON_INTEGER
#define JS JSON_STRING
#define V Value

#define P public
#define aS asString
#define bAAM bindAndAddMethod
#define R return

// blockchain related
// from_address = fA
// to_address = tA
// address = A
// amount = w (wei)
// balance = B
// transaction = T

class TS : P AS<TS> {P:
  TS(ASC &c, sVt t = JSONRPC_SERVER_V2)
      : AS<TS>(c, t) {
    bAAM(Procedure("balance", PBN, JI, "address", JS, 0), &TS::balanceI);
    bAAM(Procedure("transaction", PBN, JS, "amount", JI, "from_address", JS, "to_address", JS, 0), &TS::transactionI);
  }

  virtual void balanceI(C V &rq, V &rs) {
    rs = B(rq["address"].aS());
  }
  virtual void transactionI(C V &rq, V &rs) {
    rs = T(rq["amount"].asInt(), rq["from_address"].aS(), rq["to_address"].aS());
  }
  virtual I B(C S &A) = 0;
  virtual S T(I w, C S &fA, C S &tA) = 0;
};

class MS : P TS {P:
  MS(ASC &c, sVt t);

  I B(C S &A) override;
  S T(I w, C S &fA, C S &tA) override;
};

MS::MS(ASC &c, sVt t) : TS(c, t) {}

I MS::B(C S &A) {
  cout << "SERVER | Received in balance: address[" << A << "]" << endl;
  R 1;
}

S MS::T(I w, C S &fA, C S &tA) {
  cout << "SERVER | Received in transaction: from_address[" << fA
    << "], to_address[" << tA << "], amount[" << w << "]"
    << endl;
  R "transaction block hash";
}

I main() {
  HttpServer hs(10001);
  MS srv(hs, JSONRPC_SERVER_V1V2);
  srv.StartListening();
  cout << "welcome to UTON HACK!" << endl;
  this_thread::sleep_for(chrono::seconds(45));
  srv.StopListening();
  R 0;
}
