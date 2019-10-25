#include <stddef.h>

int ed25519_create_seed(u8 *seed);

void ed25519_create_keypair(u8 *public_key, u8 *private_key, const u8 *seed);
void ed25519_sign(u8 *signature, const u8 *message, size_t message_len, const u8 *public_key, const u8 *private_key);
int ed25519_verify(const u8 *signature, const u8 *message, size_t message_len, const u8 *public_key);
void ed25519_add_scalar(u8 *public_key, u8 *private_key, const u8 *scalar);
void ed25519_key_exchange(u8 *shared_secret, const u8 *public_key, const u8 *private_key);
