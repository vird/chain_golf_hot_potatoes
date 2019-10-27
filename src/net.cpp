u32 RPC_PRV_PORT = 10001;
u32 RPC_PUB_PORT = 10002;
string seed_ip_port = "192.168.2.3:10002";

struct NetNode {
  bool is_self = false;
  string ip_port;
};
struct NetState {
  vector<NetNode> node_list;
} gns;