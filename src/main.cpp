#define u64 unsigned long long
#define i64 long long
#define u32 unsigned int
#define i32 int
#define u8 unsigned char
#include<cstdio>
#include<filesystem>
#include<iostream>
#include<thread>
#include<jsonrpccpp/server.h>
#include<jsonrpccpp/server/connectors/httpserver.h>
#include<stdint.h>
#include<vector>
#include<cstring>

using namespace std;
using namespace jsonrpc;
using namespace Json;
// cryptography
#include "ed25519.h"
#include "fe.c"
#include "ge.c"
#include "sc.c"
#include "keypair.c"
#include "key_exchange.c"
#include "seed.c"
#include "sign.c"
#include "verify.c"
#include "sha512.c"
// blockchain stuff
#include "block.cpp"
#include "db.cpp"

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
  LOOKT_write_lookup_table_to_flash();
  unsigned char seed[32], public_key[32], private_key[64], signature[64];
  unsigned char other_public_key[32], other_private_key[64], shared_secret[32];
  const unsigned char message[] = "TEST MESSAGE";
  const int len = strlen((const char*)message);

  /* create a random seed, and a key pair out of that seed */
  if (ed25519_create_seed(seed)) {
      printf("error while generating seed\n");
      exit(1);
  }

  ed25519_create_keypair(public_key, private_key, seed);

  /* create signature on the message with the key pair */
  ed25519_sign(signature, message, len, public_key, private_key);

  /* verify the signature */
  if (ed25519_verify(signature, message, len, public_key)) {
      printf("valid signature\n");
  } else {
      printf("invalid signature\n");
  }

  /* create a dummy keypair to use for a key exchange, normally you'd only have
  the public key and receive it through some communication channel */
  if (ed25519_create_seed(seed)) {
      printf("error while generating seed\n");
      exit(1);
  }

  ed25519_create_keypair(other_public_key, other_private_key, seed);

  /* do a key exchange with other_public_key */
  ed25519_key_exchange(shared_secret, other_public_key, private_key);

  /* 
      the magic here is that ed25519_key_exchange(shared_secret, public_key,
      other_private_key); would result in the same shared_secret
  */
  // for(int i1=0;i1<32;i1++) {
  //   for(int i2=0;i2<8;i2++) {
  //     for(int i3=0;i3<10;i3++) {
  //       printf("%d ", base[i1][i2].yplusx[i3]);
  //     }
  //     printf("\n");
  //     for(int i3=0;i3<10;i3++) {
  //       printf("%d ", base[i1][i2].yminusx[i3]);
  //     }
  //     printf("\n");
  //     for(int i3=0;i3<10;i3++) {
  //       printf("%d ", base[i1][i2].xy2d[i3]);
  //     }
  //     printf("\n");
  //   }
  // }
  // HttpServer httpserver(10001);
  // MyServer s(httpserver, JSONRPC_SERVER_V1V2);
  // s.StartListening();
  // cout << "welcome to UTON HACK!" << endl;
  // this_thread::sleep_for(chrono::seconds(45));
  // s.StopListening();
  return 0;
}