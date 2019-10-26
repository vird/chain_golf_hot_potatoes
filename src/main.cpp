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
  bool work = true;
  TemplateServer(AbstractServerConnector &conn,
                 serverVersion_t type = JSONRPC_SERVER_V2)
      : AbstractServer<TemplateServer>(conn, type) {
    bindAndAddMethod(
        Procedure(
          "get_balance", PARAMS_BY_NAME, JSON_INTEGER, 
             "address", JSON_INTEGER,
               NULL),
        &TemplateServer::balanceI);
    bindAndAddMethod(
        Procedure(
          "transaction", PARAMS_BY_NAME, JSON_STRING,
            "amount", JSON_INTEGER, 
            "from_address", JSON_STRING,
            "to_address", JSON_STRING,
            NULL),
        &TemplateServer::transactionI);
  }

  void balanceI(const Value &request,
                               Value &response) {
    // const char* addr = request["address"].asString().c_str();
    // u32 addr_num = atoi(addr);
    u32 addr_num = request["address"].asInt();
    if (addr_num >= gms.balance.size()) {
      throw JsonRpcException(-1, "Account not exists");
    }
    response = gms.balance[addr_num];
  }
  void transactionI(const Value &request,
                            Value &response) {
    response = transaction( request["amount"].asInt(),
                            request["from_address"].asString(),
                            request["to_address"].asString());
  }
  string transaction(int amount,
                            const string &from_address,
                            const string &to_address) {
  cout << "SERVER | Received in transaction: from_address[" << from_address
            << "], to_address[" << to_address << "], amount[" << amount << "]"
            << endl;
  return "transaction block hash";
}
};

int main() {
  LOOKT_write_lookup_table_to_flash();
  // unsigned char seed[32] = {0};
  // if (ed25519_create_seed(seed)) {
  //   printf("error while generating seed\n");
  //   exit(1);
  // }
  // ed25519_create_keypair(my_pub_key.b, my_prv_key.b, seed);
  my_pub_key = genesis_pub_key;
  my_prv_key = genesis_priv_key_FUCK_WE_CANT_SEND_FILE_IN_OUR_SOLUTION_TO_GENESIS_NODE_FUUUUUUUUUUUUUUUUUUU;
  
  gms_init();
  
  // unsigned char public_key[32];
  // unsigned char private_key[64];
  // 
  // unsigned char seed[32] = {0};
  // unsigned char signature[64];
  // unsigned char other_public_key[32], other_private_key[64], shared_secret[32];
  // const unsigned char message[] = "TEST MESSAGE";
  // const int len = strlen((const char*)message);
  // 
  // /* create a random seed, and a key pair out of that seed */
  // if (ed25519_create_seed(seed)) {
  //     printf("error while generating seed\n");
  //     exit(1);
  // }
  // 
  // ed25519_create_keypair(public_key, private_key, seed);
  // 
  // /* create signature on the message with the key pair */
  // ed25519_sign(signature, message, len, public_key, private_key);
  // 
  // /* verify the signature */
  // if (ed25519_verify(signature, message, len, public_key)) {
  //     printf("valid signature\n");
  // } else {
  //     printf("invalid signature\n");
  // }
  // 
  // /* create a dummy keypair to use for a key exchange, normally you'd only have
  // the public key and receive it through some communication channel */
  // if (ed25519_create_seed(seed)) {
  //     printf("error while generating seed\n");
  //     exit(1);
  // }
  // 
  // ed25519_create_keypair(other_public_key, other_private_key, seed);
  // 
  // /* do a key exchange with other_public_key */
  // ed25519_key_exchange(shared_secret, other_public_key, private_key);

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
  HttpServer httpserver(10001);
  TemplateServer s(httpserver, JSONRPC_SERVER_V1V2);
  s.StartListening();
  cout << "welcome to UTON HACK!" << endl;
  while(s.work){
    block_emit();
    this_thread::sleep_for(chrono::milliseconds(10));
  }
  s.StopListening();
  return 0;
}