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
#include<ifaddrs.h>
#include<arpa/inet.h>
#include<stdint.h>
#include<algorithm>
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
#include "address.cpp"
#include "block.cpp"
#include "db.cpp"
#include "net.cpp"

void bc_height(const Value &request, Value &response) {
  response = (u64)gms.main_chain_block_list.size();
}
void get_node_list(const Value &request, Value &response) {
  for(auto it = gns.node_list.begin(), end = gns.node_list.end(); it != end; ++it) {
    Value node;
    node["ip_port_pair"] = *it;
    response.append(node);
  }
}
void get_block_number(const Value &request, Value &response) {
  i64 id = request["address"].asInt();
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
#include "local_rpc.cpp"
#include "global_rpc.cpp"

int main() {
  {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    
    getifaddrs(&ifAddrStruct);
    
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (!strcmp(addressBuffer,"127.0.0.1")) continue;
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            string addr_port = addressBuffer;
            addr_port += ":";
            addr_port += to_string(RPC_PUB_PORT);
            gns.node_list.push_back(addr_port);
        }/* else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            if (!strcmp(addressBuffer,"::1")) continue;
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            string addr_port = addressBuffer;
            addr_port += ":";
            addr_port += to_string(RPC_PUB_PORT);
            gns.node_list.push_back(addr_port);
        }*/
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  }
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
  HttpServer hs1(RPC_PRV_PORT);
  LocalServer s1(hs1, JSONRPC_SERVER_V1V2);
  HttpServer hs2(RPC_PUB_PORT);
  GlobalServer s2(hs2, JSONRPC_SERVER_V1V2);
  s1.StartListening();
  s2.StartListening();
  printf("pub server port %d\n", RPC_PUB_PORT);
  printf("prv server port %d\n", RPC_PRV_PORT);
  cout << "welcome to UTON HACK!" << endl;
  while(s1.work) {
    block_emit();
    this_thread::sleep_for(chrono::milliseconds(10));
  }
  s1.StopListening();
  s2.StopListening();
  return 0;
}