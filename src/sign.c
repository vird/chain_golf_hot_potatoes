#include "ed25519.h"
#include "sha512.h"
#include "ge.h"
#include "sc.h"


void ed25519_sign(u8 *signature, const u8 *message, size_t message_len, const u8 *public_key, const u8 *private_key) {
    sha512_context hash;
    u8 hram[64];
    u8 r[64];
    ge_p3 R;


    sha512_init(&hash);
    sha512_update(&hash, private_key + 32, 32);
    sha512_update(&hash, message, message_len);
    sha512_final(&hash, r);

    sc_reduce(r);
    ge_scalarmult_base(&R, r);
    ge_p3_tobytes(signature, &R);

    sha512_init(&hash);
    sha512_update(&hash, signature, 32);
    sha512_update(&hash, public_key, 32);
    sha512_update(&hash, message, message_len);
    sha512_final(&hash, hram);

    sc_reduce(hram);
    sc_muladd(signature + 32, hram, private_key, r);
}
