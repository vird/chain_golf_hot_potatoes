#define T_ARR(name, size) \
  const u32 name##_size = size;                                                         \
  struct name {                                                                         \
    u8 b[size] = {0};                                                                   \
  };                                                                                    \
  bool operator!=(const name& a, const name& b){return memcmp(a.b, b.b, sizeof(a.b));}  \
  void str2##name(string &str, name &key) {                                             \
    char tmp[3] = {0};                                                                  \
    char *tmp_p;                                                                        \
    u32 dst = 0;                                                                        \
    for(int i=0;i<2*name##_size;) {                                                     \
      tmp[0] = str[i++];                                                                \
      tmp[1] = str[i++];                                                                \
      key.b[dst++] = strtol((char*)&tmp, &tmp_p, 16);                                   \
    }                                                                                   \
  }                                                                                     \
  string name##2str(name &t) {                                                          \
    string res = "";                                                                    \
    char buf[10] = {0};                                                                 \
    for(int i=0;i<name##_size;i++) {                                                    \
      sprintf(buf, "%02x", t.b[i]);                                                     \
      res += buf;                                                                       \
    }                                                                                   \
    return res;                                                                         \
  }                                                                                     \
  void name##_print(const char *prefix_str, name &t) {                                  \
    printf("%s%s\n", prefix_str, name##2str(t).c_str());                                \
  }                                                                                     \

T_ARR(t_hash, 64)
T_ARR(t_sign, 64)
T_ARR(t_pub_key, 32)
T_ARR(t_prv_key, 64)

// void pub_key_print(const char *prefix_str, t_pub_key &pub_key) {
  // printf("%s", prefix_str);
  // for(int i=0;i<t_pub_key_size;i++) {
    // printf("%02x", pub_key.b[i]);
  // }
  // printf("\n");
// }
// void prv_key_print(const char *prefix_str, t_prv_key &prv_key) {
  // printf("%s", prefix_str);
  // for(int i=0;i<t_prv_key_size;i++) {
    // printf("%02x", prv_key.b[i]);
  // }
  // printf("\n");
// }


struct Block_header {
  u32       id            = 0;
  
  u32       version       = 0;
  t_hash    prev_hash     ;
  t_hash    merkle_tree   ;
  u32       issuer_addr   = 0;
  t_pub_key issuer_pub_key;
  u32       nonce         = 0;
  // end of block
  t_hash  hash;
  t_sign  sign;
  // cache
  u64     weight          = 0;
};

struct Tx {
  u32     type            = 0;
  u32     amount          = 0;
  u32     send_addr       = 0;
  u32     recv_addr       = 0;
  t_pub_key bind_pub_key  = {0};
  u32     nonce           = 0;
  // end of block
  t_hash  hash            ;
  t_sign  sign            ;
  // cache
  u64     weight          = 0;
};

struct Block {
  struct Block_header header;
  vector<Tx> tx_list;
  // cache
  u64     weight = 0;
};

const u32 tx_fee = 10;
const u32 mining_reward = 10;
const u32 hot_potato_penalty = 1;
