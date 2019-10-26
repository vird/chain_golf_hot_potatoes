#include "block.hpp"
// single block



// 1024 block pack 



// mem state
struct Acc_weight_pair {
  u32 account;
  u64 weight;
};

struct MemState {
  // TODO Block hash -> map
  u32 main_chain_block_offset = 0;
  vector<Block> main_chain_block_list;
  // tx pool
  vector<Tx> tx_list;
  
  // address_to_pub_key
  vector<t_pub_key> a2pk;
  // analog PascalCoin safebox
  vector<u32> balance;
  // consensus stuff
  vector<Acc_weight_pair*> w_weak_list;
  vector<Acc_weight_pair> aw_sort_list;
} gms;
// global_mem_state

void gms_account_new(t_pub_key &pub_key) {
  gms.a2pk.push_back(pub_key);
  gms.balance.push_back(0);
  gms.w_weak_list.push_back(NULL);
}

