#include "block.hpp"
#include "db.hpp"

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
  memcpy(buf_ptr, &header.version        , sizeof(u32      ));buf_ptr+=sizeof(u32      );   \
  memcpy(buf_ptr, &header.prev_hash      , sizeof(t_hash   ));buf_ptr+=sizeof(t_hash   );   \
  memcpy(buf_ptr, &header.merkle_tree    , sizeof(t_hash   ));buf_ptr+=sizeof(t_hash   );   \
  memcpy(buf_ptr, &header.issuer_addr    , sizeof(u32      ));buf_ptr+=sizeof(u32      );   \
  memcpy(buf_ptr, &header.issuer_pub_key , sizeof(t_pub_key));buf_ptr+=sizeof(t_pub_key);   \
  memcpy(buf_ptr, &header.nonce          , sizeof(u32      ));/*buf_ptr+=sizeof(u32      );*/ \
  sha512_context ctx;                                                                       \
  sha512_init(&ctx);                                                                        \
  sha512_update(&ctx, (u8*)&buffer, len);                                                   \


void block_header_sign(struct Block_header &header, t_pub_key &pub_key, t_prv_key &prv_key) {
  SL1
  sha512_final(&ctx, (u8*)&header.hash);
  ed25519_sign(header.sign, (u8*)&buffer, len, pub_key, prv_key);
}

bool block_header_validate(struct Block_header &header) {
  if (header.issuer_addr >= gms.a2pk.size()) return false;
  if (memcmp(header.issuer_pub_key, gms.a2pk[header.issuer_addr], sizeof(t_pub_key))) return false;
  SL1
  t_hash cmp_hash;
  sha512_final(&ctx, (u8*)&cmp_hash);
  if (memcmp(cmp_hash, header.hash, sizeof(t_hash))) return false;
  return ed25519_verify(header.sign, (u8*)&buffer, len, header.issuer_pub_key);
}

// SIGN_LEN
#define SL2 \
  const int len = (                                                     \
    sizeof(u32)+                                                        \
    sizeof(u32)+                                                        \
    sizeof(u32)+                                                        \
    sizeof(i32)                                                         \
  );                                                                    \
  u8 buffer[len];                                                       \
  u8 *buf_ptr = (u8*)&buffer;                                           \
  memcpy(buf_ptr, &tx.amount    , sizeof(u32));buf_ptr+=sizeof(u32);    \
  memcpy(buf_ptr, &tx.send_addr , sizeof(u32));buf_ptr+=sizeof(u32);    \
  memcpy(buf_ptr, &tx.recv_addr , sizeof(u32));buf_ptr+=sizeof(u32);    \
  memcpy(buf_ptr, &tx.bind_addr , sizeof(i32));/*buf_ptr+=sizeof(i32);*/  \

bool tx_validate(struct Tx &tx) {
  //acc_limit
  int L = gms.a2pk.size();
  
  if (gms.balance[tx.send_addr] < tx.amount+tx_fee) return false;
  if (tx.send_addr >= L) return false;
  if (tx.recv_addr >= L) return false;
  auto send_pub_key = gms.a2pk[tx.send_addr];
  if (tx.bind_addr != -1) {
    if (memcmp(send_pub_key, gms.a2pk[tx.send_addr], sizeof(t_pub_key))) return false;
  }
  SL2
  return ed25519_verify(tx.sign, (u8*)&buffer, len, send_pub_key);
}

void tx_sign(struct Tx &tx, t_pub_key &pub_key, t_prv_key &prv_key) {
  SL2
  ed25519_sign(tx.sign, (u8*)&buffer, len, pub_key, prv_key);
}

void tx_apply(struct Tx &tx) {
  gms.balance[tx.send_addr] -= tx.amount+tx_fee;
  gms.balance[tx.recv_addr] += tx.amount;
  
  if (tx.bind_addr != -1) {
    memcpy(gms.a2pk[tx.bind_addr], gms.a2pk[tx.recv_addr], sizeof(t_pub_key));
  }
}

// block
void merkle_tree_calc(vector<struct Tx> &tx_list, t_hash &res) {
  memset(res, 0, sizeof(t_hash));
  for(auto it = tx_list.begin(), end = tx_list.end(); it != end; ++it) {
    sha512_context ctx;
    sha512_init(&ctx);
    sha512_update(&ctx, res, sizeof(t_hash));
    sha512_update(&ctx, it->sign, sizeof(t_hash));
    sha512_final(&ctx, (u8*)&res);
  }
}

bool block_validate(struct Block &block) {
  block_header_validate(block.header);
  for(auto it = block.tx_list.begin(), end = block.tx_list.end(); it != end; ++it) {
    if (!tx_validate(*it)) return false;
  }
  
  t_hash merkle_tree;
  merkle_tree_calc(block.tx_list, merkle_tree);
  if (memcmp(merkle_tree, block.header.merkle_tree, sizeof(merkle_tree))) return false;
  return true;
}

void block_sign(struct Block &block, t_pub_key &pub_key, t_prv_key &prv_key) {
  merkle_tree_calc(block.tx_list, block.header.merkle_tree);
  block_header_sign(block.header, pub_key, prv_key);
}
void block_apply(struct Block &block) {
  // all tx
  for(auto it = block.tx_list.begin(), end = block.tx_list.end(); it != end; ++it) {
    tx_apply(*it);
  }
  // лёгкий способ быстро просуммировать все tx_fee
  gms.balance[block.header.id] += mining_reward + tx_fee*block.tx_list.size();
  // issue 1 addr
  // gms.a2pk.push_back(block.header.issuer_pub_key);
  // gms.balance.push_back(0);
}