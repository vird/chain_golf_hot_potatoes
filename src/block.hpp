typedef u8 t_hash[128];
typedef u8 t_sign[64];
typedef u8 t_pub_key[32];
typedef u8 t_prv_key[64];

struct Block_header {
  u32       version       ;
  t_hash    prev_hash     ;
  t_hash    merkle_tree   ;
  t_pub_key issuer_pub_key;
  u32       nonce         ;
  // end of block
  t_hash hash;
  t_sign sign;
};

struct Tx {
  u32     amount   ;
  u32     send_addr;
  u32     recv_addr;
  i32     bind_addr;
  t_sign  sign     ;
};

struct Block {
  struct Block_header header;
  vector<Tx> tx_list;
};