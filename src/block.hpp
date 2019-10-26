#define T_ARR(name, size) \
  const u32 name##_size = size; \
  struct name {\
    u8 b[size];\
  };\
  bool operator!=(const name& a, const name& b){return memcmp(a.b, b.b, sizeof(a.b));}

T_ARR(t_hash, 64)
T_ARR(t_sign, 64)
T_ARR(t_pub_key, 32)
T_ARR(t_prv_key, 64)

// TODO move to T_ARR
void str2t_pub_key(string &str, t_pub_key &key) {
  char tmp[3] = {0};
  char *tmp_p;
  u32 dst = 0;
  for(int i=0;i<2*t_pub_key_size;) {
    tmp[0] = str[i++];
    tmp[1] = str[i++];
    key.b[dst++] = strtol((char*)&tmp, &tmp_p, 16);
  }
}
void str2t_prv_key(string &str, t_prv_key &key) {
  char tmp[3] = {0};
  char *tmp_p;
  u32 dst = 0;
  for(int i=0;i<2*t_prv_key_size;) {
    tmp[0] = str[i++];
    tmp[1] = str[i++];
    key.b[dst++] = strtol((char*)&tmp, &tmp_p, 16);
  }
}
void pub_key_print(const char *prefix_str, t_pub_key &pub_key) {
  printf("%s", prefix_str);
  for(int i=0;i<t_pub_key_size;i++) {
    printf("%02x", pub_key.b[i]);
  }
  printf("\n");
}
void prv_key_print(const char *prefix_str, t_prv_key &prv_key) {
  printf("%s", prefix_str);
  for(int i=0;i<t_prv_key_size;i++) {
    printf("%02x", prv_key.b[i]);
  }
  printf("\n");
}


struct Block_header {
  u32       id;
  
  u32       version       ;
  t_hash    prev_hash     ;
  t_hash    merkle_tree   ;
  u32       issuer_addr   ;
  t_pub_key issuer_pub_key;
  u32       nonce         ;
  // end of block
  t_hash  hash;
  t_sign  sign;
  // cache
  u64     weight;
};

struct Tx {
  u32     type     ;
  u32     amount   ;
  u32     send_addr;
  u32     recv_addr;
  t_pub_key bind_pub_key = {0};
  u32     nonce    ;
  // end of block
  t_hash  hash     ;
  t_sign  sign     ;
  // cache
  u64     weight   ;
};

struct Block {
  struct Block_header header;
  vector<Tx> tx_list;
  // cache
  u64     weight;
};

const u32 tx_fee = 1;
const u32 mining_reward = 1;