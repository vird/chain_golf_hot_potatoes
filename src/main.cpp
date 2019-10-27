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
#include<unordered_map>
#include<cstring>
#include<getopt.h>
#define FOR_COL(it, arr) for(auto it = arr.begin(), end = arr.end(); it != end; ++it)

using namespace std;
using namespace jsonrpc;
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
// util
#include "fs.cpp"
// blockchain stuff
#include "address.cpp"
#include "block.cpp"
#include "db.cpp"
// net
#include "rpc_util.cpp"
#include "rpc_server_local.cpp"
#include "rpc_server_global.cpp"
/* #include "rpc_client.cpp" */
#define throw(...)
#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#undef throw

#include "net.cpp"

int main(int argc, char **argv) {
  // ed25519 calc table
  LOOKT_write_lookup_table_to_flash();
  
  //argv
  int option_index = 0;
  static struct option long_options[] = {
    //name              has_arg flag val
    {"rpc_pub_port",      1,      0,  0  },
    {"rpc_prv_port",      1,      0,  0  },
    {"seed_ip_port",      1,      0,  0  },
    {"pub_key_path",      1,      0,  0  },
    {"prv_key_path",      1,      0,  0  },
    {"drop_keys",         0,      0,  0  },
    {0, 0, 0, 0}
  };
  
  bool drop_keys = false;
  while (1) {
    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) break;
    
    switch (option_index) {
      case 0:
        RPC_PUB_PORT = atoi(optarg);
        break;
      case 1:
        RPC_PRV_PORT = atoi(optarg);
        break;
      case 2:
        seed_ip_port = optarg;
        break;
      case 3:
        pub_key_path = optarg;
        break;
      case 4:
        prv_key_path = optarg;
        break;
      case 5:
        drop_keys = true;
        break;
    }
  }
  
  if (drop_keys) {
    printf("drop keys\n");
    remove(pub_key_path.c_str());
    remove(prv_key_path.c_str());
  }

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
            if (!strcmp(addressBuffer, "127.0.0.1")) continue;
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            string addr_port = "http://";
            addr_port += addressBuffer;
            addr_port += ":";
            addr_port += to_string(RPC_PUB_PORT);
            NetNode node;
            node.is_self = true;
            node.ip_port = addr_port;
            gns.node_list.push_back(node);
        }/* else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            if (!strcmp(addressBuffer,"::1")) continue;
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            string addr_port = "http://[";
            addr_port += addressBuffer;
            addr_port += "]:";
            addr_port += to_string(RPC_PUB_PORT);
            NetNode node;
            node.is_self = true;
            node.ip_port = addr_port;
            gns.node_list.push_back(node);
        }*/
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  }
  
  FOR_COL(it, gns.node_list) {
    if (it->ip_port == seed_ip_port) {
      i_am_seed_node = true;
    }
  }
  
  bool pub_exists = file_exists(pub_key_path);
  bool prv_exists = file_exists(prv_key_path);
  if (pub_exists && prv_exists) {
    // read
    printf("load keys\n");
    if (!file_load(pub_key_path, my_pub_key.b, t_pub_key_size)) {
      printf("failed to load pub key\n");
      return 1;
    }
    if (!file_load(prv_key_path, my_prv_key.b, t_prv_key_size)) {
      printf("failed to load prv key\n");
      return 1;
    }
  } else if (!pub_exists && !prv_exists)  {
    // create
    printf("generate keys\n");
    if (!key_gen(my_pub_key, my_prv_key)) {
      printf("error while generating keypair");
      return 1;
    }
    printf("save keys\n");
    if (!file_save(pub_key_path, my_pub_key.b, t_pub_key_size)) {
      printf("failed to save pub key\n");
      return 1;
    }
    if (!file_save(prv_key_path, my_prv_key.b, t_prv_key_size)) {
      printf("failed to save prv key\n");
      return 1;
    }
  } else {
    printf("invalid situation\n");
    printf("pub_key %s\n", pub_exists?"present":"missing");
    printf("prv_key %s\n", prv_exists?"present":"missing");
    return 1;
  }
  if (i_am_seed_node) {
    gms_init();
  } else {
    NetNode node;
    node.ip_port = seed_ip_port;
    gns.node_list.push_back(node);
  }
  
  HttpServer hs1(RPC_PRV_PORT);
  LocalServer s1(hs1, JSONRPC_SERVER_V1V2);
  HttpServer hs2(RPC_PUB_PORT);
  GlobalServer s2(hs2, JSONRPC_SERVER_V1V2);
  s1.StartListening();
  s2.StartListening();
  printf("pub server port %d\n", RPC_PUB_PORT);
  printf("prv server port %d\n", RPC_PRV_PORT);
  printf("seed_ip_port___ %s\n", seed_ip_port.c_str());
  printf("i_am_seed_node_ %d\n", i_am_seed_node);
  printf("welcome to UTON HACK!\n");
  u32 last_bc = 0;
  while(s1.work) {
    net_tick();
    if (!gms.ready) {
      u32 new_bc = bc_height();
      if (last_bc == new_bc) {
        this_thread::sleep_for(chrono::milliseconds(1000));
      } else {
        last_bc = new_bc;
        this_thread::sleep_for(chrono::milliseconds(1));
      }
      printf("node is not ready bc_height = %d / %d\n", bc_height(), gms.target_bc_height);
    } else {
      // block propose
      block_propose();
      // subticks
      for(int i=0;i<5;i++) {
        net_tick();
        this_thread::sleep_for(chrono::milliseconds(20));
      }
      // block commit
      // Все кто хотел должны были уже договориться...
      printf("new block %d\n", gms.proposed_block.header.id);
      if (!gms.is_proposal_valid) {
        throw new Exception("bad assert gms.is_proposal_valid");
      }
      block_apply(gms.proposed_block);
      gms.is_proposal_valid = false;
    }
  }
  s1.StopListening();
  s2.StopListening();
  return 0;
}
