#include "block.hpp"
string pub_key_path = "./pub.key";
string prv_key_path = "./prv.key";
bool i_am_seed_node = false;
// single block



// 1024 block pack 



// mem state
struct Acc_weight_pair {
  u32 account;
  u64 weight;
};

struct MemState {
  bool ready = false;
  u32 target_bc_height = 0;
  // TODO Block hash -> map
  u32 main_chain_block_offset = 0;
  vector<Block> main_chain_block_list;
  // tx prepared by this node
  vector<Tx> tx_list;
  
  Block proposed_block;
  unordered_map<string, Tx> done_tx_hash;
  // address_to_pub_key
  vector<t_pub_key> a2pk;
  // analog PascalCoin safebox
  vector<u32> balance;
  // consensus stuff
  vector<Acc_weight_pair*> w_weak_list;
  vector<Acc_weight_pair> aw_sort_list;
} gms;
// global_mem_state

u32 my_primary_address;
t_pub_key my_pub_key;
t_prv_key my_prv_key;
bool tx_mining_mode = false;

void gms_account_new(t_pub_key &pub_key) {
  gms.a2pk.push_back(pub_key);
  gms.balance.push_back(0);
  gms.w_weak_list.push_back(NULL);
}

u32 bc_height() {
  return gms.main_chain_block_offset + gms.main_chain_block_list.size();
}

bool key_gen(t_pub_key &pub_key, t_prv_key &prv_key) {
  u8 seed[32];
  if (ed25519_create_seed(seed)) {
    return false;
  }
  ed25519_create_keypair(pub_key.b, prv_key.b, seed);
  return true;
}
