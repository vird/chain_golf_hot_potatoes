#include "block.hpp"
#include "db.hpp"

u64 hash2weight(t_hash &hash) {
  u64 res = 0;
  for(int i=0;i<t_hash_size;i++) {
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
  memcpy(buf_ptr, &header.id                , s=sizeof(header.id              ));buf_ptr+=s;   \
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

void block_header_to_json(Block_header &header, Value &value) {
  value["id"]             = header.id                           ;
  value["version"]        = header.version                      ;
  value["prev_hash"]      = t_hash2str(header.prev_hash)        ;
  value["merkle_tree"]    = t_hash2str(header.merkle_tree)      ;
  value["issuer_addr"]    = header.issuer_addr                  ;
  value["issuer_pub_key"] = t_pub_key2str(header.issuer_pub_key);
  value["nonce"]          = header.nonce                        ;
  value["hash"]           = t_hash2str(header.hash)             ;
  value["sign"]           = t_sign2str(header.sign)             ;
  value["weight"]         = header.weight                       ;
}

// SIGN_LEN
#define SL2 \
  const int len = (                                                     \
    sizeof(u32)+                                                        \
    sizeof(u32)+                                                        \
    sizeof(u32)+                                                        \
    sizeof(u32)+                                                        \
    sizeof(u32)                                                         \
  );                                                                    \
  u8 buffer[len];                                                       \
  u8 *buf_ptr = (u8*)&buffer;                                           \
  u32 s = 0;                                                            \
  memcpy(buf_ptr, &tx.type      , s=sizeof(u32));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.amount    , s=sizeof(u32));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.send_addr , s=sizeof(u32));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.recv_addr , s=sizeof(u32));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.bind_pub_key.b, s=sizeof(t_pub_key_size));buf_ptr+=s;            \
  memcpy(buf_ptr, &tx.nonce     , s=sizeof(u32));/*buf_ptr+=s;*/        \
  sha512_context ctx;                                                   \
  sha512_init(&ctx);                                                    \
  sha512_update(&ctx, (u8*)&buffer, len);                               \

void tx_sign(Tx &tx, t_pub_key &pub_key, t_prv_key &prv_key) {
  SL2
  sha512_final(&ctx, (u8*)&tx.hash);
  ed25519_sign(tx.sign.b, (u8*)&buffer, len, pub_key.b, prv_key.b);
}

int tx_validate_reason = 0;
#define RET(x) {tx_validate_reason=x;return false;}
bool tx_validate(Tx &tx) {
  //acc_limit
  int L = gms.a2pk.size();
  if (tx.send_addr >= L) RET(1)
  if (tx.recv_addr >= L) RET(2)
  auto send_pub_key = gms.a2pk[tx.send_addr];
  
  switch(tx.type) {
    case 1: // transfer
      // overflow protection
      if (gms.balance[tx.send_addr] < max(tx.amount, tx.amount + tx_fee)) RET(10)
      break;
    case 2: // address_transfer
      if (gms.balance[tx.send_addr] < tx_fee) RET(20)
      if (tx.recv_addr >= L) RET(21)
      if (send_pub_key != gms.a2pk[tx.recv_addr]) RET(22)
      break;
    default:
      RET(30)
  }
  
  SL2
  t_hash cmp_hash;
  sha512_final(&ctx, (u8*)&cmp_hash);
  if (cmp_hash != tx.hash) RET(3)
  if (!ed25519_verify(tx.sign.b, (u8*)&buffer, len, send_pub_key.b)) RET(4)
  return true;
}

void tx_apply(Tx &tx) {
  switch(tx.type) {
    case 1: // transfer
      gms.balance[tx.send_addr] -= tx.amount+tx_fee;
      gms.balance[tx.recv_addr] += tx.amount;
      break;
    
    case 2: // address_transfer
      gms.balance[tx.send_addr] -= tx_fee;
      gms.a2pk[tx.recv_addr] = tx.bind_pub_key;
      break;
  }
}

void tx_to_json(Tx &tx, Value &value) {
  value["type"]     = tx.type      ;
  value["amount"]   = tx.amount    ;
  value["send_addr"]= tx.send_addr ;
  value["recv_addr"]= tx.recv_addr ;
  value["bind_pub_key"]= t_pub_key2str(tx.bind_pub_key);
  value["nonce"]    = tx.nonce     ;
}

// block
void merkle_tree_calc(vector<Tx> &tx_list, t_hash &res) {
  memset(res.b, 0, sizeof(t_hash));
  FOR_COL(it, tx_list) {
    sha512_context ctx;
    sha512_init(&ctx);
    sha512_update(&ctx, res.b, sizeof(res.b));
    sha512_update(&ctx, it->sign.b, sizeof(res.b));
    sha512_final(&ctx, (u8*)&res.b);
  }
}

bool block_validate(Block &block) {
  block_header_validate(block.header);
  FOR_COL(it, block.tx_list) {
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
  FOR_COL(it, block.tx_list) {
    tx_apply(*it);
  }
  // лёгкий способ быстро просуммировать все tx_fee
  gms.balance[block.header.issuer_addr] += mining_reward + tx_fee*block.tx_list.size();
  // issue 1 addr
  gms_account_new(block.header.issuer_pub_key);
}

void block_weight_calc(Block &block) {
  u32 weight = 0;
  FOR_COL(it, block.tx_list) {
    weight += it->weight = hash2weight(it->hash);
  }
  weight += block.header.weight = hash2weight(block.header.hash);
  block.weight = weight;
}

void block_to_json(Block &block, Value &value) {
  Value header;
  block_header_to_json(block.header, header);
  Value tx_list;
  FOR_COL(it, block.tx_list) {
    Value tx;
    tx_to_json(*it, tx);
    tx_list.append(tx);
  }
  
  value["header"] = header;
  value["tx_list"] = tx_list;
  value["weight"] = block.weight;
}