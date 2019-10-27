#include "db.hpp"
// single block
int block_pack_size(Block &block) {
  int res = 0;
  
  return res;
}
void block_pack(Block &block) {
  
}
void block_unpack(Block &block) {
  
}

// 1024 block pack 



// mem state
void gms_init() {
  gms_account_new(my_pub_key);
  gms.balance[0] = 1e6;
  
  Block block;
  block.header.id = 0;
  block.header.version = 1;
  block.header.issuer_addr = 0;
  block.header.issuer_pub_key = my_pub_key;
  block.header.nonce = 0;
  block_sign(block, my_pub_key, my_prv_key);
  block_weight_calc(block);
  if (!block_validate(block)) {
    throw new Exception("block validation failed for our own block");
  }
  block_apply(block);
  gms.main_chain_block_list.push_back(block);
  gms.ready = true;
}

void block_emit() {
  if (!gms.ready) return;
  auto last = gms.main_chain_block_list.back();
  Block block;
  block.header.id = last.header.id+1;
  block.header.version = 1;
  block.header.prev_hash = last.header.hash;
  block.header.issuer_addr = my_primary_address;
  block.header.issuer_pub_key = my_pub_key;
  block.header.nonce = 0;
  block.tx_list = gms.tx_list;
  gms.tx_list.clear();
  block_sign(block, my_pub_key, my_prv_key);
  block_weight_calc(block);
  gms.main_chain_block_list.push_back(block);
  if (!block_validate(block)) {
    throw new Exception("block validation failed for our own block");
  }
  block_apply(block);
  if (block.header.id % 100 == 0) {
    printf("block id=%d\n", block.header.id);
  }
}