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
  TemplateServer(AbstractServerConnector &conn, serverVersion_t type = JSONRPC_SERVER_V2) : AbstractServer<TemplateServer>(conn, type) {
    bindAndAddMethod(Procedure(
      "get_balance", PARAMS_BY_NAME, JSON_INTEGER,
         "address", JSON_INTEGER,
           NULL),
      &TemplateServer::balanceI);
    bindAndAddMethod(Procedure(
      "shutdown", PARAMS_BY_NAME, JSON_INTEGER,
           NULL),
      &TemplateServer::shutdownI);
    bindAndAddMethod(Procedure(
      "transfer", PARAMS_BY_NAME, JSON_INTEGER,
        "amount", JSON_INTEGER,
        "from_address", JSON_INTEGER,
        "to_address", JSON_INTEGER,
        NULL),
      &TemplateServer::transferI);
    bindAndAddMethod(Procedure(
      "address_transfer", PARAMS_BY_NAME, JSON_INTEGER,
        "address", JSON_INTEGER,
        "pub_key", JSON_STRING,
        NULL),
      &TemplateServer::address_transferI);
    // good for debug
    bindAndAddMethod(Procedure(
      "debug_set_key", PARAMS_BY_NAME, JSON_INTEGER,
        "address", JSON_INTEGER,
        "pub_key", JSON_STRING,
        "prv_key", JSON_STRING,
        NULL),
      &TemplateServer::debug_set_keyI);
    bindAndAddMethod(Procedure(
      "debug_key_gen", PARAMS_BY_NAME, JSON_INTEGER,
        NULL),
      &TemplateServer::debug_key_genI);
  }
  
  void balanceI(const Value &request, Value &response) {
    // const char* addr = request["address"].asString().c_str();
    // u32 address = atoi(addr);
    u32 address = request["address"].asInt();
    if (address >= gms.balance.size()) {
      throw JsonRpcException(-1, "Address not exists");
    }
    response = gms.balance[address];
  }
  
  void shutdownI(const Value &request, Value &response) {
    printf("shutdown scheduled\n");
    work = false;
    response = 1;
  }
  
  void transferI(const Value &request, Value &response) {
    u32 amount = request["amount"].asInt();
    
    u32 from_address = request["from_address"].asInt();
    if (from_address >= gms.balance.size()) {
      throw JsonRpcException(-1, "from_address not exists");
    }
    u32 to_address = request["to_address"].asInt();
    if (to_address >= gms.balance.size()) {
      throw JsonRpcException(-1, "to_address not exists");
    }
    if (gms.a2pk[from_address] != my_pub_key) {
      throw JsonRpcException(-2, "you don't own from_address");
    }
    // overflow protection
    if (gms.balance[from_address] < max(amount, amount + tx_fee)) {
      throw JsonRpcException(-3, "not enough balance");
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
      throw JsonRpcException(-9, "tx_validate fail");
    }
    
    printf("tx transfer %d coin %d -> %d\n", amount, from_address, to_address);
    
    gms.tx_list.push_back(tx);
    
    response = 1;
  }
  
  void address_transferI(const Value &request, Value &response) {
    u32 address = request["address"].asInt();
    if (gms.a2pk[address] != my_pub_key) {
      throw JsonRpcException(-2, "you don't own address");
    }
    string hex_pub_key = request["pub_key"].asString();
    if (hex_pub_key.size() != 2*t_pub_key_size) {
      throw JsonRpcException(-1, "bad pub_key size");
    }
    for(int i=0;i<2*t_pub_key_size;i++) {
      char ch = hex_pub_key[i];
      if ('0' <= ch && ch <= '9') continue;
      if ('a' <= ch && ch <= 'f') continue;
      if ('A' <= ch && ch <= 'A') continue;
      throw JsonRpcException(-1, "bad pub_key hex");
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
      throw JsonRpcException(-9, "tx_validate fail");
    }
    
    printf("address transfer owner=%d address=%d pub_key=%s\n", my_primary_address, address, hex_pub_key.c_str());
    
    gms.tx_list.push_back(tx);
    
    response = 1;
  }
  
  void debug_set_keyI(const Value &request, Value &response) {
    u32 address = request["address"].asInt();
    if (address >= gms.balance.size()) {
      throw JsonRpcException(-1, "Address not exists");
    }
    
    // TODO move to define
    string hex_pub_key = request["pub_key"].asString();
    if (hex_pub_key.size() != 2*t_pub_key_size) {
      throw JsonRpcException(-1, "bad pub_key size");
    }
    for(int i=0;i<2*t_pub_key_size;i++) {
      char ch = hex_pub_key[i];
      if ('0' <= ch && ch <= '9') continue;
      if ('a' <= ch && ch <= 'f') continue;
      if ('A' <= ch && ch <= 'A') continue;
      throw JsonRpcException(-1, "bad pub_key hex");
    }
    
    string hex_prv_key = request["prv_key"].asString();
    if (hex_prv_key.size() != 2*t_prv_key_size) {
      throw JsonRpcException(-1, "bad prv_key size");
    }
    for(int i=0;i<2*t_prv_key_size;i++) {
      char ch = hex_prv_key[i];
      if ('0' <= ch && ch <= '9') continue;
      if ('a' <= ch && ch <= 'f') continue;
      if ('A' <= ch && ch <= 'A') continue;
      throw JsonRpcException(-1, "bad prv_key hex");
    }
    
    t_pub_key pub_key;
    t_prv_key prv_key;
    
    str2t_pub_key(hex_pub_key, pub_key);
    str2t_prv_key(hex_prv_key, prv_key);
    
    if (gms.a2pk[address] != pub_key) {
      pub_key_print("pub_key           = ", pub_key);
      pub_key_print("gms.a2pk[address] = ", gms.a2pk[address]);
      throw JsonRpcException(-2, "address and pub_key mismatch");
    }
    
    my_primary_address = address;
    my_pub_key = pub_key;
    my_prv_key = prv_key;
    pub_key_print("my_pub_key           = ", my_pub_key);
    prv_key_print("my_prv_key           = ", my_prv_key);
    
    response = 1;
  }
  
  void debug_key_genI(const Value &request, Value &response) {
    unsigned char seed[32];
    if (ed25519_create_seed(seed)) {
      printf("error while generating seed\n");
      exit(1);
    }
    t_pub_key pub_key;
    t_prv_key prv_key;
    ed25519_create_keypair(pub_key.b, prv_key.b, seed);
    
    pub_key_print("pub_key = ", pub_key);
    prv_key_print("prv_key = ", prv_key);
    
    response = 1;
  }
};

int main() {
  LOOKT_write_lookup_table_to_flash();
  gms_init();
  
  // unsigned char seed[32] = {0};
  // if (ed25519_create_seed(seed)) {
  //   printf("error while generating seed\n");
  //   exit(1);
  // }
  // ed25519_create_keypair(my_pub_key.b, my_prv_key.b, seed);
  my_primary_address = 0;
  my_pub_key = genesis_pub_key;
  my_prv_key = genesis_priv_key_FUCK_WE_CANT_SEND_FILE_IN_OUR_SOLUTION_TO_GENESIS_NODE_FUUUUUUUUUUUUUUUUUUU;
  
  
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