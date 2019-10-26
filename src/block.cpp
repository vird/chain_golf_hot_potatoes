#include "block.hpp"
#include "db.hpp"

u64 hash2weight(t_hash &hash) {
  u64 res = 0;
  for(int i=0;i<hash_byte_count;i++) {
    u32 loc = __builtin_clz(hash.b[i]);
    res += loc;
    if (loc != 32) break;
  }
  return res;
}

// SIGN_LEN
#define SL1 \
  const int len = (                                                                         \
    sizeof(u32      )+                                                                      \
    sizeof(t_hash   )+                                                                      \
    sizeof(t_hash   )+                                                                      \
    sizeof(u32      )+                                                                      \
    sizeof(t_pub_key)+                                                                      \
    sizeof(u32      )                                                                       \
  );                                                                                        \
  u8 buffer[len];                                                                           \
  u8 *buf_ptr = (u8*)&buffer;                                                               \
  u32 s = 0;                                                                               \
  memcpy(buf_ptr, &header.version           , s=sizeof(header.version         ));buf_ptr+=s;   \
  memcpy(buf_ptr, &header.prev_hash         , s=sizeof(header.prev_hash       ));buf_ptr+=s;   \
  memcpy(buf_ptr, &header.merkle_tree       , s=sizeof(header.merkle_tree     ));buf_ptr+=s;   \
  memcpy(buf_ptr, &header.issuer_addr       , s=sizeof(header.issuer_addr     ));buf_ptr+=s;   \
  memcpy(buf_ptr, &header.issuer_pub_key.b  , s=sizeof(header.issuer_pub_key.b));buf_ptr+=s;   \
  memcpy(buf_ptr, &header.nonce             , s=sizeof(header.nonce           ));/*buf_ptr+=s;*/\
  sha512_context ctx;                                                                       \
  sha512_init(&ctx);                                                                        \
  sha512_update(&ctx, (u8*)&buffer, len);                                                   \


void block_header_sign(Block_header &header, t_pub_key &pub_key, t_prv_key &prv_key) {
  SL1
  sha512_final(&ctx, (u8*)&header.hash);
  ed25519_sign(header.sign.b, (u8*)&buffer, len, pub_key.b, prv_key.b);
}

bool block_header_validate(Block_header &header) {
  if (header.issuer_addr >= gms.a2pk.size()) return false;
  if (header.issuer_pub_key != gms.a2pk[header.issuer_addr]) return false;
  SL1
  t_hash cmp_hash;
  sha512_final(&ctx, (u8*)&cmp_hash);
  if (cmp_hash != header.hash) return false;
  return ed25519_verify(header.sign.b, (u8*)&buffer, len, header.issuer_pub_key.b);
}

// SIGN_LEN
#define SL2 \
  const int len = (                                                     \
    sizeof(u32)+                                                        \
    sizeof(u32)+                                                        \
    sizeof(u32)+                                                        \
    sizeof(i32)+                                                        \
    sizeof(u32)                                                         \
  );                                                                    \
  u8 buffer[len];                                                       \
  u8 *buf_ptr = (u8*)&buffer;                                           \
  u32 s = 0;                                                            \
  memcpy(buf_ptr, &tx.amount    , s=sizeof(u32));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.send_addr , s=sizeof(u32));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.recv_addr , s=sizeof(u32));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.bind_addr , s=sizeof(i32));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.nonce     , s=sizeof(u32));/*buf_ptr+=s;*/        \
  sha512_context ctx;                                                   \
  sha512_init(&ctx);                                                    \
  sha512_update(&ctx, (u8*)&buffer, len);                               \

void tx_sign(Tx &tx, t_pub_key &pub_key, t_prv_key &prv_key) {
  SL2
  sha512_final(&ctx, (u8*)&tx.hash);
  ed25519_sign(tx.sign.b, (u8*)&buffer, len, pub_key.b, prv_key.b);
}

bool tx_validate(Tx &tx) {
  //acc_limit
  int L = gms.a2pk.size();
  
  if (gms.balance[tx.send_addr] < tx.amount+tx_fee) return false;
  if (tx.send_addr >= L) return false;
  if (tx.recv_addr >= L) return false;
  auto send_pub_key = gms.a2pk[tx.send_addr];
  if (tx.bind_addr != -1) {
    if (send_pub_key != gms.a2pk[tx.send_addr]) return false;
  }
  SL2
  t_hash cmp_hash;
  sha512_final(&ctx, (u8*)&cmp_hash);
  if (cmp_hash != tx.hash) return false;
  return ed25519_verify(tx.sign.b, (u8*)&buffer, len, send_pub_key.b);
}

void tx_apply(Tx &tx) {
  gms.balance[tx.send_addr] -= tx.amount+tx_fee;
  gms.balance[tx.recv_addr] += tx.amount;
  
  if (tx.bind_addr != -1) {
    gms.a2pk[tx.bind_addr] = gms.a2pk[tx.recv_addr];
  }
}

// block
void merkle_tree_calc(vector<Tx> &tx_list, t_hash &res) {
  memset(res.b, 0, sizeof(t_hash));
  for(auto it = tx_list.begin(), end = tx_list.end(); it != end; ++it) {
    sha512_context ctx;
    sha512_init(&ctx);
    sha512_update(&ctx, res.b, sizeof(res.b));
    sha512_update(&ctx, it->sign.b, sizeof(res.b));
    sha512_final(&ctx, (u8*)&res.b);
  }
}

bool block_validate(Block &block) {
  block_header_validate(block.header);
  for(auto it = block.tx_list.begin(), end = block.tx_list.end(); it != end; ++it) {
    if (!tx_validate(*it)) return false;
  }
  
  t_hash merkle_tree;
  merkle_tree_calc(block.tx_list, merkle_tree);
  if (merkle_tree != block.header.merkle_tree) return false;
  return true;
}

void block_sign(Block &block, t_pub_key &pub_key, t_prv_key &prv_key) {
  merkle_tree_calc(block.tx_list, block.header.merkle_tree);
  block_header_sign(block.header, pub_key, prv_key);
}
void block_apply(Block &block) {
  // all tx
  for(auto it = block.tx_list.begin(), end = block.tx_list.end(); it != end; ++it) {
    tx_apply(*it);
  }
  // лёгкий способ быстро просуммировать все tx_fee
  gms.balance[block.header.issuer_addr] += mining_reward + tx_fee*block.tx_list.size();
  // issue 1 addr
  gms_account_new(block.header.issuer_pub_key);
}

void block_weight_calc(Block &block) {
  u32 weight = 0;
  for(auto it = block.tx_list.begin(), end = block.tx_list.end(); it != end; ++it) {
    weight += it->weight = hash2weight(it->hash);
  }
  weight += block.header.weight = hash2weight(block.header.hash);
  block.weight = weight;
}