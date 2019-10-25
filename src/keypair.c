#include "ed25519.h"
#include "sha512.h"
#include "ge.h"


void ed25519_create_keypair(u8 *public_key, u8 *private_key, const u8 *seed) {
    ge_p3 A;

    sha512(seed, 32, private_key);
    private_key[0] &= 248;
    private_key[31] &= 63;
    private_key[31] |= 64;

    ge_scalarmult_base(&A, private_key);
    ge_p3_tobytes(public_key, &A);
}
