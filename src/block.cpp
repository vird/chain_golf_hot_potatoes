#include "block.hpp"
#include "db.hpp"

// SIGN_LEN
#define SL1 \
  const int len = (                                                                         \
    sizeof(u32      )+                                                                      \
    sizeof(t_hash   )+                                                                      \
    sizeof(t_hash   )+                                                                      \
    sizeof(t_pub_key)+                                                                      \
    sizeof(u32      )                                                                       \
  );                                                                                        \
  u8 buffer[len];                                                                           \
  u8 *buf_ptr = (u8*)&buffer;                                                               \
  memcpy(buf_ptr, &header.version        , sizeof(u32      ));buf_ptr+=sizeof(u32      );  \
  memcpy(buf_ptr, &header.prev_hash      , sizeof(t_hash   ));buf_ptr+=sizeof(t_hash   );  \
  memcpy(buf_ptr, &header.merkle_tree    , sizeof(t_hash   ));buf_ptr+=sizeof(t_hash   );  \
  memcpy(buf_ptr, &header.issuer_pub_key , sizeof(t_pub_key));buf_ptr+=sizeof(t_pub_key);  \
  memcpy(buf_ptr, &header.nonce          , sizeof(u32      ));//buf_ptr+=sizeof(u32      );\


void block_header_sign(struct Block_header &header, t_pub_key &pub_key, t_prv_key &prv_key) {
  SL1
  ed25519_sign(header.sign, (u8*)&buffer, len, pub_key, prv_key);
}

bool block_header_validate(struct Block_header &header) {
  SL1
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
  memcpy(buf_ptr, &tx.bind_addr , sizeof(i32));//buf_ptr+=sizeof(i32);  \

bool tx_validate(struct Tx &tx) {
  int acc_limit = gms.a2pk.size();
  
  if (gms.balance[tx.send_addr] < tx.amount) return false;
  if (tx.send_addr >= acc_limit) return false;
  if (tx.recv_addr >= acc_limit) return false;
  auto send_pub_key = gms.a2pk[tx.send_addr];
  if (tx.bind_addr != -1) {
    if (!memcmp(send_pub_key, gms.a2pk[tx.send_addr], sizeof(t_pub_key))) return false;
  }
  SL2
  return ed25519_verify(tx.sign, (u8*)&buffer, len, send_pub_key);
}

void tx_sign(struct Tx &tx, t_pub_key &pub_key, t_prv_key &prv_key) {
  auto map_ptr = &gms.a2pk;
  SL2
  ed25519_sign(tx.sign, (u8*)&buffer, len, pub_key, prv_key);
}

void tx_apply(struct Tx &tx) {
  gms.balance[tx.send_addr] -= tx.amount;
  gms.balance[tx.recv_addr] += tx.amount;
  
  if (tx.bind_addr != -1) {
    memcpy(gms.a2pk[tx.bind_addr], gms.a2pk[tx.recv_addr], sizeof(t_pub_key));
  }
}