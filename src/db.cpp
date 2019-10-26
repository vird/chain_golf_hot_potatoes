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
u32 my_primary_address = 0;
t_pub_key my_pub_key;
t_prv_key my_prv_key;


t_pub_key genesis_pub_key;
t_prv_key genesis_priv_key_FUCK_WE_CANT_SEND_FILE_IN_OUR_SOLUTION_TO_GENESIS_NODE_FUUUUUUUUUUUUUUUUUUU;

void gms_init() {
  unsigned char seed[32] = {0xFC};
  ed25519_create_keypair(genesis_pub_key.b, genesis_priv_key_FUCK_WE_CANT_SEND_FILE_IN_OUR_SOLUTION_TO_GENESIS_NODE_FUUUUUUUUUUUUUUUUUUU.b, seed);
  
  gms_account_new(genesis_pub_key);
  gms.balance[0] = 1e6;
  
  Block block;
  block.header.id = 0;
  block.header.version = 1;
  block.header.issuer_addr = 0;
  block.header.issuer_pub_key = genesis_pub_key;
  block.header.nonce = 0;
  block_sign(block, genesis_pub_key, genesis_priv_key_FUCK_WE_CANT_SEND_FILE_IN_OUR_SOLUTION_TO_GENESIS_NODE_FUUUUUUUUUUUUUUUUUUU);
  block_weight_calc(block);
  if (!block_validate(block)) {
    throw new Exception("block validation failed for our own block");
  }
  block_apply(block);
  gms.main_chain_block_list.push_back(block);
}

void block_emit() {
  Block block;
  block.header.id = gms.main_chain_block_list.back().header.id+1;
  block.header.version = 1;
  block.header.issuer_addr = my_primary_address;
  block.header.issuer_pub_key = my_pub_key;
  block.header.nonce = 0;
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