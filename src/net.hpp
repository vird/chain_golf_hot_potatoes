u32 RPC_PRV_PORT = 10002;
u32 RPC_PUB_PORT = 10001;
string seed_ip_port = "http://192.168.2.3:10001";

struct NetNode {
  bool is_self = false;
  string ip_port;
  bool is_proposal_valid = false;
  Block proposed_block;
};
struct NetState {
  u32 ask_offset = 0;
  u32 broadcast_offset = 0;
  vector<NetNode> node_list;
} gns;

void block_broadcast();