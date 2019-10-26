#define T_ARR(name, size) \
  struct name {\
    u8 b[size];\
  };\
  bool operator!=(const name& a, const name& b){return memcmp(a.b, b.b, sizeof(a.b));}

T_ARR(t_hash, 64)
T_ARR(t_sign, 64)
T_ARR(t_pub_key, 32)
T_ARR(t_prv_key, 64)

struct Block_header {
  u32       id;
  
  u32       version       ;
  t_hash    prev_hash     ;
  t_hash    merkle_tree   ;
  u32       issuer_addr   ;
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

const u32 tx_fee = 1;
const u32 mining_reward = 1;