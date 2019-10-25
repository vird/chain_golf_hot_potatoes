#include <stddef.h>

#include "fixedint.h"

/* state */
typedef struct sha512_context_ {
    u64  length, state[8];
    size_t curlen;
    u8 buf[128];
} sha512_context;


int sha512_init(sha512_context * md);
int sha512_final(sha512_context * md, u8 *out);
int sha512_update(sha512_context * md, const u8 *in, size_t inlen);
int sha512(const u8 *message, size_t message_len, u8 *out);
