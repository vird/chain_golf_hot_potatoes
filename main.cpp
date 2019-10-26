#define C const
#define I i64
#define Q i32
#define AS AbstractServer
#define ASC AbstractServerConnector
#define bM bindAndAddMethod
#define sVt serverVersion_t
#define JS JSON_STRING
#define V virtual
#define U using namespace
#define u64 unsigned long long
#define i64 long long
#define u32 unsigned int
#define i32 int
#define u8 unsigned char
#include<cstdio>
#include<filesystem>
#include<iostream>
#include<thread>
#include<jsonrpccpp/server.h>
#include<jsonrpccpp/server/connectors/httpserver.h>
#include<stdint.h>
#include<vector>
#include<cstring>

using namespace std;
using namespace jsonrpc;
using namespace Json;
// cryptography
#include <stddef.h>

int ed25519_create_seed(u8 *seed);

void ed25519_create_keypair(u8 *public_key, u8 *private_key, const u8 *seed);
void ed25519_sign(u8 *signature, const u8 *message, size_t message_len, const u8 *public_key, const u8 *private_key);
int ed25519_verify(const u8 *signature, const u8 *message, size_t message_len, const u8 *public_key);
void ed25519_add_scalar(u8 *public_key, u8 *private_key, const u8 *scalar);
void ed25519_key_exchange(u8 *shared_secret, const u8 *public_key, const u8 *private_key);





/*
    fe means field element.
    Here the field is \Z/(2^255-19).
    An element t, entries t[0]...t[9], represents the integer
    t[0]+2^26 t[1]+2^51 t[2]+2^77 t[3]+2^102 t[4]+...+2^230 t[9].
    Bounds on each t[i] vary depending on context.
*/


typedef i32 fe[10];


void fe_0(fe h);
void fe_1(fe h);

void fe_frombytes(fe h, const u8 *s);
void fe_tobytes(u8 *s, const fe h);

void fe_copy(fe h, const fe f);
int fe_isnegative(const fe f);
int fe_isnonzero(const fe f);
void fe_cmov(fe f, const fe g, u32 b);
void fe_cswap(fe f, fe g, u32 b);

void fe_neg(fe h, const fe f);
void fe_add(fe h, const fe f, const fe g);
void fe_invert(fe out, const fe z);
void fe_sq(fe h, const fe f);
void fe_sq2(fe h, const fe f);
void fe_mul(fe h, const fe f, const fe g);
void fe_mul121666(fe h, fe f);
void fe_pow22523(fe out, const fe z);
void fe_sub(fe h, const fe f, const fe g);



/*
    helper functions
*/
static u64 load_3(const u8 *in) {
    u64 result;

    result = (u64) in[0];
    result |= ((u64) in[1]) << 8;
    result |= ((u64) in[2]) << 16;

    return result;
}

static u64 load_4(const u8 *in) {
    u64 result;

    result = (u64) in[0];
    result |= ((u64) in[1]) << 8;
    result |= ((u64) in[2]) << 16;
    result |= ((u64) in[3]) << 24;
    
    return result;
}



/*
    h = 0
*/

void fe_0(fe h) {
    h[0] = 0;
    h[1] = 0;
    h[2] = 0;
    h[3] = 0;
    h[4] = 0;
    h[5] = 0;
    h[6] = 0;
    h[7] = 0;
    h[8] = 0;
    h[9] = 0;
}



/*
    h = 1
*/

void fe_1(fe h) {
    h[0] = 1;
    h[1] = 0;
    h[2] = 0;
    h[3] = 0;
    h[4] = 0;
    h[5] = 0;
    h[6] = 0;
    h[7] = 0;
    h[8] = 0;
    h[9] = 0;
}



/*
    h = f + g
    Can overlap h with f or g.

    Preconditions:
       |f| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.
       |g| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.

    Postconditions:
       |h| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.
*/

void fe_add(fe h, const fe f, const fe g) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i32 g0 = g[0];
    i32 g1 = g[1];
    i32 g2 = g[2];
    i32 g3 = g[3];
    i32 g4 = g[4];
    i32 g5 = g[5];
    i32 g6 = g[6];
    i32 g7 = g[7];
    i32 g8 = g[8];
    i32 g9 = g[9];
    i32 h0 = f0 + g0;
    i32 h1 = f1 + g1;
    i32 h2 = f2 + g2;
    i32 h3 = f3 + g3;
    i32 h4 = f4 + g4;
    i32 h5 = f5 + g5;
    i32 h6 = f6 + g6;
    i32 h7 = f7 + g7;
    i32 h8 = f8 + g8;
    i32 h9 = f9 + g9;
    
    h[0] = h0;
    h[1] = h1;
    h[2] = h2;
    h[3] = h3;
    h[4] = h4;
    h[5] = h5;
    h[6] = h6;
    h[7] = h7;
    h[8] = h8;
    h[9] = h9;
}



/*
    Replace (f,g) with (g,g) if b == 1;
    replace (f,g) with (f,g) if b == 0.

    Preconditions: b in {0,1}.
*/

void fe_cmov(fe f, const fe g, u32 b) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i32 g0 = g[0];
    i32 g1 = g[1];
    i32 g2 = g[2];
    i32 g3 = g[3];
    i32 g4 = g[4];
    i32 g5 = g[5];
    i32 g6 = g[6];
    i32 g7 = g[7];
    i32 g8 = g[8];
    i32 g9 = g[9];
    i32 x0 = f0 ^ g0;
    i32 x1 = f1 ^ g1;
    i32 x2 = f2 ^ g2;
    i32 x3 = f3 ^ g3;
    i32 x4 = f4 ^ g4;
    i32 x5 = f5 ^ g5;
    i32 x6 = f6 ^ g6;
    i32 x7 = f7 ^ g7;
    i32 x8 = f8 ^ g8;
    i32 x9 = f9 ^ g9;

    b = (u32) (- (int) b); /* silence warning */
    x0 &= b;
    x1 &= b;
    x2 &= b;
    x3 &= b;
    x4 &= b;
    x5 &= b;
    x6 &= b;
    x7 &= b;
    x8 &= b;
    x9 &= b;
    
    f[0] = f0 ^ x0;
    f[1] = f1 ^ x1;
    f[2] = f2 ^ x2;
    f[3] = f3 ^ x3;
    f[4] = f4 ^ x4;
    f[5] = f5 ^ x5;
    f[6] = f6 ^ x6;
    f[7] = f7 ^ x7;
    f[8] = f8 ^ x8;
    f[9] = f9 ^ x9;
}

/*
    Replace (f,g) with (g,f) if b == 1;
    replace (f,g) with (f,g) if b == 0.

    Preconditions: b in {0,1}.
*/

void fe_cswap(fe f,fe g,u32 b) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i32 g0 = g[0];
    i32 g1 = g[1];
    i32 g2 = g[2];
    i32 g3 = g[3];
    i32 g4 = g[4];
    i32 g5 = g[5];
    i32 g6 = g[6];
    i32 g7 = g[7];
    i32 g8 = g[8];
    i32 g9 = g[9];
    i32 x0 = f0 ^ g0;
    i32 x1 = f1 ^ g1;
    i32 x2 = f2 ^ g2;
    i32 x3 = f3 ^ g3;
    i32 x4 = f4 ^ g4;
    i32 x5 = f5 ^ g5;
    i32 x6 = f6 ^ g6;
    i32 x7 = f7 ^ g7;
    i32 x8 = f8 ^ g8;
    i32 x9 = f9 ^ g9;
    b = (u32) (- (int) b); /* silence warning */
    x0 &= b;
    x1 &= b;
    x2 &= b;
    x3 &= b;
    x4 &= b;
    x5 &= b;
    x6 &= b;
    x7 &= b;
    x8 &= b;
    x9 &= b;
    f[0] = f0 ^ x0;
    f[1] = f1 ^ x1;
    f[2] = f2 ^ x2;
    f[3] = f3 ^ x3;
    f[4] = f4 ^ x4;
    f[5] = f5 ^ x5;
    f[6] = f6 ^ x6;
    f[7] = f7 ^ x7;
    f[8] = f8 ^ x8;
    f[9] = f9 ^ x9;
    g[0] = g0 ^ x0;
    g[1] = g1 ^ x1;
    g[2] = g2 ^ x2;
    g[3] = g3 ^ x3;
    g[4] = g4 ^ x4;
    g[5] = g5 ^ x5;
    g[6] = g6 ^ x6;
    g[7] = g7 ^ x7;
    g[8] = g8 ^ x8;
    g[9] = g9 ^ x9;
}



/*
    h = f
*/

void fe_copy(fe h, const fe f) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    
    h[0] = f0;
    h[1] = f1;
    h[2] = f2;
    h[3] = f3;
    h[4] = f4;
    h[5] = f5;
    h[6] = f6;
    h[7] = f7;
    h[8] = f8;
    h[9] = f9;
}



/*
    Ignores top bit of h.
*/

void fe_frombytes(fe h, const u8 *s) {
    i64 h0 = load_4(s);
    i64 h1 = load_3(s + 4) << 6;
    i64 h2 = load_3(s + 7) << 5;
    i64 h3 = load_3(s + 10) << 3;
    i64 h4 = load_3(s + 13) << 2;
    i64 h5 = load_4(s + 16);
    i64 h6 = load_3(s + 20) << 7;
    i64 h7 = load_3(s + 23) << 5;
    i64 h8 = load_3(s + 26) << 4;
    i64 h9 = (load_3(s + 29) & 8388607) << 2;
    i64 carry0;
    i64 carry1;
    i64 carry2;
    i64 carry3;
    i64 carry4;
    i64 carry5;
    i64 carry6;
    i64 carry7;
    i64 carry8;
    i64 carry9;

    carry9 = (h9 + (i64) (1 << 24)) >> 25;
    h0 += carry9 * 19;
    h9 -= carry9 << 25;
    carry1 = (h1 + (i64) (1 << 24)) >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry3 = (h3 + (i64) (1 << 24)) >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry5 = (h5 + (i64) (1 << 24)) >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;
    carry7 = (h7 + (i64) (1 << 24)) >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;
    carry0 = (h0 + (i64) (1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry2 = (h2 + (i64) (1 << 25)) >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry4 = (h4 + (i64) (1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry6 = (h6 + (i64) (1 << 25)) >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;
    carry8 = (h8 + (i64) (1 << 25)) >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;

    h[0] = (i32) h0;
    h[1] = (i32) h1;
    h[2] = (i32) h2;
    h[3] = (i32) h3;
    h[4] = (i32) h4;
    h[5] = (i32) h5;
    h[6] = (i32) h6;
    h[7] = (i32) h7;
    h[8] = (i32) h8;
    h[9] = (i32) h9;
}



void fe_invert(fe out, const fe z) {
    fe t0;
    fe t1;
    fe t2;
    fe t3;
    int i;

    fe_sq(t0, z);

    for (i = 1; i < 1; ++i) {
        fe_sq(t0, t0);
    }

    fe_sq(t1, t0);

    for (i = 1; i < 2; ++i) {
        fe_sq(t1, t1);
    }

    fe_mul(t1, z, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t2, t0);

    for (i = 1; i < 1; ++i) {
        fe_sq(t2, t2);
    }

    fe_mul(t1, t1, t2);
    fe_sq(t2, t1);

    for (i = 1; i < 5; ++i) {
        fe_sq(t2, t2);
    }

    fe_mul(t1, t2, t1);
    fe_sq(t2, t1);

    for (i = 1; i < 10; ++i) {
        fe_sq(t2, t2);
    }

    fe_mul(t2, t2, t1);
    fe_sq(t3, t2);

    for (i = 1; i < 20; ++i) {
        fe_sq(t3, t3);
    }

    fe_mul(t2, t3, t2);
    fe_sq(t2, t2);

    for (i = 1; i < 10; ++i) {
        fe_sq(t2, t2);
    }

    fe_mul(t1, t2, t1);
    fe_sq(t2, t1);

    for (i = 1; i < 50; ++i) {
        fe_sq(t2, t2);
    }

    fe_mul(t2, t2, t1);
    fe_sq(t3, t2);

    for (i = 1; i < 100; ++i) {
        fe_sq(t3, t3);
    }

    fe_mul(t2, t3, t2);
    fe_sq(t2, t2);

    for (i = 1; i < 50; ++i) {
        fe_sq(t2, t2);
    }

    fe_mul(t1, t2, t1);
    fe_sq(t1, t1);

    for (i = 1; i < 5; ++i) {
        fe_sq(t1, t1);
    }

    fe_mul(out, t1, t0);
}



/*
    return 1 if f is in {1,3,5,...,q-2}
    return 0 if f is in {0,2,4,...,q-1}

    Preconditions:
       |f| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.
*/

int fe_isnegative(const fe f) {
    u8 s[32];

    fe_tobytes(s, f);
    
    return s[0] & 1;
}



/*
    return 1 if f == 0
    return 0 if f != 0

    Preconditions:
       |f| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.
*/
// FUCK!!!!
// #define F(i) r |= s[i]

int fe_isnonzero(const fe f) {
    u8 s[32];
    u8 r;

    fe_tobytes(s, f);

    r = s[0];
    #define F(i) r |= s[i]
    F(1);
    F(2);
    F(3);
    F(4);
    F(5);
    F(6);
    F(7);
    F(8);
    F(9);
    F(10);
    F(11);
    F(12);
    F(13);
    F(14);
    F(15);
    F(16);
    F(17);
    F(18);
    F(19);
    F(20);
    F(21);
    F(22);
    F(23);
    F(24);
    F(25);
    F(26);
    F(27);
    F(28);
    F(29);
    F(30);
    F(31);
    #undef F

    return r != 0;
}



/*
    h = f * g
    Can overlap h with f or g.

    Preconditions:
       |f| bounded by 1.65*2^26,1.65*2^25,1.65*2^26,1.65*2^25,etc.
       |g| bounded by 1.65*2^26,1.65*2^25,1.65*2^26,1.65*2^25,etc.

    Postconditions:
       |h| bounded by 1.01*2^25,1.01*2^24,1.01*2^25,1.01*2^24,etc.
    */

    /*
    Notes on implementation strategy:

    Using schoolbook multiplication.
    Karatsuba would save a little in some cost models.

    Most multiplications by 2 and 19 are 32-bit precomputations;
    cheaper than 64-bit postcomputations.

    There is one remaining multiplication by 19 in the carry chain;
    one *19 precomputation can be merged into this,
    but the resulting data flow is considerably less clean.

    There are 12 carries below.
    10 of them are 2-way parallelizable and vectorizable.
    Can get away with 11 carries, but then data flow is much deeper.

    With tighter constraints on inputs can squeeze carries into int32.
*/

void fe_mul(fe h, const fe f, const fe g) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i32 g0 = g[0];
    i32 g1 = g[1];
    i32 g2 = g[2];
    i32 g3 = g[3];
    i32 g4 = g[4];
    i32 g5 = g[5];
    i32 g6 = g[6];
    i32 g7 = g[7];
    i32 g8 = g[8];
    i32 g9 = g[9];
    i32 g1_19 = 19 * g1; /* 1.959375*2^29 */
    i32 g2_19 = 19 * g2; /* 1.959375*2^30; still ok */
    i32 g3_19 = 19 * g3;
    i32 g4_19 = 19 * g4;
    i32 g5_19 = 19 * g5;
    i32 g6_19 = 19 * g6;
    i32 g7_19 = 19 * g7;
    i32 g8_19 = 19 * g8;
    i32 g9_19 = 19 * g9;
    i32 f1_2 = 2 * f1;
    i32 f3_2 = 2 * f3;
    i32 f5_2 = 2 * f5;
    i32 f7_2 = 2 * f7;
    i32 f9_2 = 2 * f9;
    i64 f0g0    = f0   * (i64) g0;
    i64 f0g1    = f0   * (i64) g1;
    i64 f0g2    = f0   * (i64) g2;
    i64 f0g3    = f0   * (i64) g3;
    i64 f0g4    = f0   * (i64) g4;
    i64 f0g5    = f0   * (i64) g5;
    i64 f0g6    = f0   * (i64) g6;
    i64 f0g7    = f0   * (i64) g7;
    i64 f0g8    = f0   * (i64) g8;
    i64 f0g9    = f0   * (i64) g9;
    i64 f1g0    = f1   * (i64) g0;
    i64 f1g1_2  = f1_2 * (i64) g1;
    i64 f1g2    = f1   * (i64) g2;
    i64 f1g3_2  = f1_2 * (i64) g3;
    i64 f1g4    = f1   * (i64) g4;
    i64 f1g5_2  = f1_2 * (i64) g5;
    i64 f1g6    = f1   * (i64) g6;
    i64 f1g7_2  = f1_2 * (i64) g7;
    i64 f1g8    = f1   * (i64) g8;
    i64 f1g9_38 = f1_2 * (i64) g9_19;
    i64 f2g0    = f2   * (i64) g0;
    i64 f2g1    = f2   * (i64) g1;
    i64 f2g2    = f2   * (i64) g2;
    i64 f2g3    = f2   * (i64) g3;
    i64 f2g4    = f2   * (i64) g4;
    i64 f2g5    = f2   * (i64) g5;
    i64 f2g6    = f2   * (i64) g6;
    i64 f2g7    = f2   * (i64) g7;
    i64 f2g8_19 = f2   * (i64) g8_19;
    i64 f2g9_19 = f2   * (i64) g9_19;
    i64 f3g0    = f3   * (i64) g0;
    i64 f3g1_2  = f3_2 * (i64) g1;
    i64 f3g2    = f3   * (i64) g2;
    i64 f3g3_2  = f3_2 * (i64) g3;
    i64 f3g4    = f3   * (i64) g4;
    i64 f3g5_2  = f3_2 * (i64) g5;
    i64 f3g6    = f3   * (i64) g6;
    i64 f3g7_38 = f3_2 * (i64) g7_19;
    i64 f3g8_19 = f3   * (i64) g8_19;
    i64 f3g9_38 = f3_2 * (i64) g9_19;
    i64 f4g0    = f4   * (i64) g0;
    i64 f4g1    = f4   * (i64) g1;
    i64 f4g2    = f4   * (i64) g2;
    i64 f4g3    = f4   * (i64) g3;
    i64 f4g4    = f4   * (i64) g4;
    i64 f4g5    = f4   * (i64) g5;
    i64 f4g6_19 = f4   * (i64) g6_19;
    i64 f4g7_19 = f4   * (i64) g7_19;
    i64 f4g8_19 = f4   * (i64) g8_19;
    i64 f4g9_19 = f4   * (i64) g9_19;
    i64 f5g0    = f5   * (i64) g0;
    i64 f5g1_2  = f5_2 * (i64) g1;
    i64 f5g2    = f5   * (i64) g2;
    i64 f5g3_2  = f5_2 * (i64) g3;
    i64 f5g4    = f5   * (i64) g4;
    i64 f5g5_38 = f5_2 * (i64) g5_19;
    i64 f5g6_19 = f5   * (i64) g6_19;
    i64 f5g7_38 = f5_2 * (i64) g7_19;
    i64 f5g8_19 = f5   * (i64) g8_19;
    i64 f5g9_38 = f5_2 * (i64) g9_19;
    i64 f6g0    = f6   * (i64) g0;
    i64 f6g1    = f6   * (i64) g1;
    i64 f6g2    = f6   * (i64) g2;
    i64 f6g3    = f6   * (i64) g3;
    i64 f6g4_19 = f6   * (i64) g4_19;
    i64 f6g5_19 = f6   * (i64) g5_19;
    i64 f6g6_19 = f6   * (i64) g6_19;
    i64 f6g7_19 = f6   * (i64) g7_19;
    i64 f6g8_19 = f6   * (i64) g8_19;
    i64 f6g9_19 = f6   * (i64) g9_19;
    i64 f7g0    = f7   * (i64) g0;
    i64 f7g1_2  = f7_2 * (i64) g1;
    i64 f7g2    = f7   * (i64) g2;
    i64 f7g3_38 = f7_2 * (i64) g3_19;
    i64 f7g4_19 = f7   * (i64) g4_19;
    i64 f7g5_38 = f7_2 * (i64) g5_19;
    i64 f7g6_19 = f7   * (i64) g6_19;
    i64 f7g7_38 = f7_2 * (i64) g7_19;
    i64 f7g8_19 = f7   * (i64) g8_19;
    i64 f7g9_38 = f7_2 * (i64) g9_19;
    i64 f8g0    = f8   * (i64) g0;
    i64 f8g1    = f8   * (i64) g1;
    i64 f8g2_19 = f8   * (i64) g2_19;
    i64 f8g3_19 = f8   * (i64) g3_19;
    i64 f8g4_19 = f8   * (i64) g4_19;
    i64 f8g5_19 = f8   * (i64) g5_19;
    i64 f8g6_19 = f8   * (i64) g6_19;
    i64 f8g7_19 = f8   * (i64) g7_19;
    i64 f8g8_19 = f8   * (i64) g8_19;
    i64 f8g9_19 = f8   * (i64) g9_19;
    i64 f9g0    = f9   * (i64) g0;
    i64 f9g1_38 = f9_2 * (i64) g1_19;
    i64 f9g2_19 = f9   * (i64) g2_19;
    i64 f9g3_38 = f9_2 * (i64) g3_19;
    i64 f9g4_19 = f9   * (i64) g4_19;
    i64 f9g5_38 = f9_2 * (i64) g5_19;
    i64 f9g6_19 = f9   * (i64) g6_19;
    i64 f9g7_38 = f9_2 * (i64) g7_19;
    i64 f9g8_19 = f9   * (i64) g8_19;
    i64 f9g9_38 = f9_2 * (i64) g9_19;
    i64 h0 = f0g0 + f1g9_38 + f2g8_19 + f3g7_38 + f4g6_19 + f5g5_38 + f6g4_19 + f7g3_38 + f8g2_19 + f9g1_38;
    i64 h1 = f0g1 + f1g0   + f2g9_19 + f3g8_19 + f4g7_19 + f5g6_19 + f6g5_19 + f7g4_19 + f8g3_19 + f9g2_19;
    i64 h2 = f0g2 + f1g1_2 + f2g0   + f3g9_38 + f4g8_19 + f5g7_38 + f6g6_19 + f7g5_38 + f8g4_19 + f9g3_38;
    i64 h3 = f0g3 + f1g2   + f2g1   + f3g0   + f4g9_19 + f5g8_19 + f6g7_19 + f7g6_19 + f8g5_19 + f9g4_19;
    i64 h4 = f0g4 + f1g3_2 + f2g2   + f3g1_2 + f4g0   + f5g9_38 + f6g8_19 + f7g7_38 + f8g6_19 + f9g5_38;
    i64 h5 = f0g5 + f1g4   + f2g3   + f3g2   + f4g1   + f5g0   + f6g9_19 + f7g8_19 + f8g7_19 + f9g6_19;
    i64 h6 = f0g6 + f1g5_2 + f2g4   + f3g3_2 + f4g2   + f5g1_2 + f6g0   + f7g9_38 + f8g8_19 + f9g7_38;
    i64 h7 = f0g7 + f1g6   + f2g5   + f3g4   + f4g3   + f5g2   + f6g1   + f7g0   + f8g9_19 + f9g8_19;
    i64 h8 = f0g8 + f1g7_2 + f2g6   + f3g5_2 + f4g4   + f5g3_2 + f6g2   + f7g1_2 + f8g0   + f9g9_38;
    i64 h9 = f0g9 + f1g8   + f2g7   + f3g6   + f4g5   + f5g4   + f6g3   + f7g2   + f8g1   + f9g0   ;
    i64 carry0;
    i64 carry1;
    i64 carry2;
    i64 carry3;
    i64 carry4;
    i64 carry5;
    i64 carry6;
    i64 carry7;
    i64 carry8;
    i64 carry9;

    carry0 = (h0 + (i64) (1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry4 = (h4 + (i64) (1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;

    carry1 = (h1 + (i64) (1 << 24)) >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry5 = (h5 + (i64) (1 << 24)) >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;

    carry2 = (h2 + (i64) (1 << 25)) >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry6 = (h6 + (i64) (1 << 25)) >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;

    carry3 = (h3 + (i64) (1 << 24)) >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry7 = (h7 + (i64) (1 << 24)) >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;

    carry4 = (h4 + (i64) (1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry8 = (h8 + (i64) (1 << 25)) >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;

    carry9 = (h9 + (i64) (1 << 24)) >> 25;
    h0 += carry9 * 19;
    h9 -= carry9 << 25;

    carry0 = (h0 + (i64) (1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;

    h[0] = (i32) h0;
    h[1] = (i32) h1;
    h[2] = (i32) h2;
    h[3] = (i32) h3;
    h[4] = (i32) h4;
    h[5] = (i32) h5;
    h[6] = (i32) h6;
    h[7] = (i32) h7;
    h[8] = (i32) h8;
    h[9] = (i32) h9;
}


/*
h = f * 121666
Can overlap h with f.

Preconditions:
   |f| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.

Postconditions:
   |h| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.
*/

void fe_mul121666(fe h, fe f) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i64 h0 = f0 * (i64) 121666;
    i64 h1 = f1 * (i64) 121666;
    i64 h2 = f2 * (i64) 121666;
    i64 h3 = f3 * (i64) 121666;
    i64 h4 = f4 * (i64) 121666;
    i64 h5 = f5 * (i64) 121666;
    i64 h6 = f6 * (i64) 121666;
    i64 h7 = f7 * (i64) 121666;
    i64 h8 = f8 * (i64) 121666;
    i64 h9 = f9 * (i64) 121666;
    i64 carry0;
    i64 carry1;
    i64 carry2;
    i64 carry3;
    i64 carry4;
    i64 carry5;
    i64 carry6;
    i64 carry7;
    i64 carry8;
    i64 carry9;

    carry9 = (h9 + (i64) (1<<24)) >> 25; h0 += carry9 * 19; h9 -= carry9 << 25;
    carry1 = (h1 + (i64) (1<<24)) >> 25; h2 += carry1; h1 -= carry1 << 25;
    carry3 = (h3 + (i64) (1<<24)) >> 25; h4 += carry3; h3 -= carry3 << 25;
    carry5 = (h5 + (i64) (1<<24)) >> 25; h6 += carry5; h5 -= carry5 << 25;
    carry7 = (h7 + (i64) (1<<24)) >> 25; h8 += carry7; h7 -= carry7 << 25;

    carry0 = (h0 + (i64) (1<<25)) >> 26; h1 += carry0; h0 -= carry0 << 26;
    carry2 = (h2 + (i64) (1<<25)) >> 26; h3 += carry2; h2 -= carry2 << 26;
    carry4 = (h4 + (i64) (1<<25)) >> 26; h5 += carry4; h4 -= carry4 << 26;
    carry6 = (h6 + (i64) (1<<25)) >> 26; h7 += carry6; h6 -= carry6 << 26;
    carry8 = (h8 + (i64) (1<<25)) >> 26; h9 += carry8; h8 -= carry8 << 26;

    h[0] = (i32) h0;
    h[1] = (i32) h1;
    h[2] = (i32) h2;
    h[3] = (i32) h3;
    h[4] = (i32) h4;
    h[5] = (i32) h5;
    h[6] = (i32) h6;
    h[7] = (i32) h7;
    h[8] = (i32) h8;
    h[9] = (i32) h9;
}


/*
h = -f

Preconditions:
   |f| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.

Postconditions:
   |h| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.
*/

void fe_neg(fe h, const fe f) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i32 h0 = -f0;
    i32 h1 = -f1;
    i32 h2 = -f2;
    i32 h3 = -f3;
    i32 h4 = -f4;
    i32 h5 = -f5;
    i32 h6 = -f6;
    i32 h7 = -f7;
    i32 h8 = -f8;
    i32 h9 = -f9;

    h[0] = h0;
    h[1] = h1;
    h[2] = h2;
    h[3] = h3;
    h[4] = h4;
    h[5] = h5;
    h[6] = h6;
    h[7] = h7;
    h[8] = h8;
    h[9] = h9;
}


void fe_pow22523(fe out, const fe z) {
    fe t0;
    fe t1;
    fe t2;
    int i;
    fe_sq(t0, z);

    for (i = 1; i < 1; ++i) {
        fe_sq(t0, t0);
    }

    fe_sq(t1, t0);

    for (i = 1; i < 2; ++i) {
        fe_sq(t1, t1);
    }

    fe_mul(t1, z, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t0, t0);

    for (i = 1; i < 1; ++i) {
        fe_sq(t0, t0);
    }

    fe_mul(t0, t1, t0);
    fe_sq(t1, t0);

    for (i = 1; i < 5; ++i) {
        fe_sq(t1, t1);
    }

    fe_mul(t0, t1, t0);
    fe_sq(t1, t0);

    for (i = 1; i < 10; ++i) {
        fe_sq(t1, t1);
    }

    fe_mul(t1, t1, t0);
    fe_sq(t2, t1);

    for (i = 1; i < 20; ++i) {
        fe_sq(t2, t2);
    }

    fe_mul(t1, t2, t1);
    fe_sq(t1, t1);

    for (i = 1; i < 10; ++i) {
        fe_sq(t1, t1);
    }

    fe_mul(t0, t1, t0);
    fe_sq(t1, t0);

    for (i = 1; i < 50; ++i) {
        fe_sq(t1, t1);
    }

    fe_mul(t1, t1, t0);
    fe_sq(t2, t1);

    for (i = 1; i < 100; ++i) {
        fe_sq(t2, t2);
    }

    fe_mul(t1, t2, t1);
    fe_sq(t1, t1);

    for (i = 1; i < 50; ++i) {
        fe_sq(t1, t1);
    }

    fe_mul(t0, t1, t0);
    fe_sq(t0, t0);

    for (i = 1; i < 2; ++i) {
        fe_sq(t0, t0);
    }

    fe_mul(out, t0, z);
    return;
}


/*
h = f * f
Can overlap h with f.

Preconditions:
   |f| bounded by 1.65*2^26,1.65*2^25,1.65*2^26,1.65*2^25,etc.

Postconditions:
   |h| bounded by 1.01*2^25,1.01*2^24,1.01*2^25,1.01*2^24,etc.
*/

/*
See fe_mul.c for discussion of implementation strategy.
*/

void fe_sq(fe h, const fe f) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i32 f0_2 = 2 * f0;
    i32 f1_2 = 2 * f1;
    i32 f2_2 = 2 * f2;
    i32 f3_2 = 2 * f3;
    i32 f4_2 = 2 * f4;
    i32 f5_2 = 2 * f5;
    i32 f6_2 = 2 * f6;
    i32 f7_2 = 2 * f7;
    i32 f5_38 = 38 * f5; /* 1.959375*2^30 */
    i32 f6_19 = 19 * f6; /* 1.959375*2^30 */
    i32 f7_38 = 38 * f7; /* 1.959375*2^30 */
    i32 f8_19 = 19 * f8; /* 1.959375*2^30 */
    i32 f9_38 = 38 * f9; /* 1.959375*2^30 */
    i64 f0f0    = f0   * (i64) f0;
    i64 f0f1_2  = f0_2 * (i64) f1;
    i64 f0f2_2  = f0_2 * (i64) f2;
    i64 f0f3_2  = f0_2 * (i64) f3;
    i64 f0f4_2  = f0_2 * (i64) f4;
    i64 f0f5_2  = f0_2 * (i64) f5;
    i64 f0f6_2  = f0_2 * (i64) f6;
    i64 f0f7_2  = f0_2 * (i64) f7;
    i64 f0f8_2  = f0_2 * (i64) f8;
    i64 f0f9_2  = f0_2 * (i64) f9;
    i64 f1f1_2  = f1_2 * (i64) f1;
    i64 f1f2_2  = f1_2 * (i64) f2;
    i64 f1f3_4  = f1_2 * (i64) f3_2;
    i64 f1f4_2  = f1_2 * (i64) f4;
    i64 f1f5_4  = f1_2 * (i64) f5_2;
    i64 f1f6_2  = f1_2 * (i64) f6;
    i64 f1f7_4  = f1_2 * (i64) f7_2;
    i64 f1f8_2  = f1_2 * (i64) f8;
    i64 f1f9_76 = f1_2 * (i64) f9_38;
    i64 f2f2    = f2   * (i64) f2;
    i64 f2f3_2  = f2_2 * (i64) f3;
    i64 f2f4_2  = f2_2 * (i64) f4;
    i64 f2f5_2  = f2_2 * (i64) f5;
    i64 f2f6_2  = f2_2 * (i64) f6;
    i64 f2f7_2  = f2_2 * (i64) f7;
    i64 f2f8_38 = f2_2 * (i64) f8_19;
    i64 f2f9_38 = f2   * (i64) f9_38;
    i64 f3f3_2  = f3_2 * (i64) f3;
    i64 f3f4_2  = f3_2 * (i64) f4;
    i64 f3f5_4  = f3_2 * (i64) f5_2;
    i64 f3f6_2  = f3_2 * (i64) f6;
    i64 f3f7_76 = f3_2 * (i64) f7_38;
    i64 f3f8_38 = f3_2 * (i64) f8_19;
    i64 f3f9_76 = f3_2 * (i64) f9_38;
    i64 f4f4    = f4   * (i64) f4;
    i64 f4f5_2  = f4_2 * (i64) f5;
    i64 f4f6_38 = f4_2 * (i64) f6_19;
    i64 f4f7_38 = f4   * (i64) f7_38;
    i64 f4f8_38 = f4_2 * (i64) f8_19;
    i64 f4f9_38 = f4   * (i64) f9_38;
    i64 f5f5_38 = f5   * (i64) f5_38;
    i64 f5f6_38 = f5_2 * (i64) f6_19;
    i64 f5f7_76 = f5_2 * (i64) f7_38;
    i64 f5f8_38 = f5_2 * (i64) f8_19;
    i64 f5f9_76 = f5_2 * (i64) f9_38;
    i64 f6f6_19 = f6   * (i64) f6_19;
    i64 f6f7_38 = f6   * (i64) f7_38;
    i64 f6f8_38 = f6_2 * (i64) f8_19;
    i64 f6f9_38 = f6   * (i64) f9_38;
    i64 f7f7_38 = f7   * (i64) f7_38;
    i64 f7f8_38 = f7_2 * (i64) f8_19;
    i64 f7f9_76 = f7_2 * (i64) f9_38;
    i64 f8f8_19 = f8   * (i64) f8_19;
    i64 f8f9_38 = f8   * (i64) f9_38;
    i64 f9f9_38 = f9   * (i64) f9_38;
    i64 h0 = f0f0  + f1f9_76 + f2f8_38 + f3f7_76 + f4f6_38 + f5f5_38;
    i64 h1 = f0f1_2 + f2f9_38 + f3f8_38 + f4f7_38 + f5f6_38;
    i64 h2 = f0f2_2 + f1f1_2 + f3f9_76 + f4f8_38 + f5f7_76 + f6f6_19;
    i64 h3 = f0f3_2 + f1f2_2 + f4f9_38 + f5f8_38 + f6f7_38;
    i64 h4 = f0f4_2 + f1f3_4 + f2f2   + f5f9_76 + f6f8_38 + f7f7_38;
    i64 h5 = f0f5_2 + f1f4_2 + f2f3_2 + f6f9_38 + f7f8_38;
    i64 h6 = f0f6_2 + f1f5_4 + f2f4_2 + f3f3_2 + f7f9_76 + f8f8_19;
    i64 h7 = f0f7_2 + f1f6_2 + f2f5_2 + f3f4_2 + f8f9_38;
    i64 h8 = f0f8_2 + f1f7_4 + f2f6_2 + f3f5_4 + f4f4   + f9f9_38;
    i64 h9 = f0f9_2 + f1f8_2 + f2f7_2 + f3f6_2 + f4f5_2;
    i64 carry0;
    i64 carry1;
    i64 carry2;
    i64 carry3;
    i64 carry4;
    i64 carry5;
    i64 carry6;
    i64 carry7;
    i64 carry8;
    i64 carry9;
    carry0 = (h0 + (i64) (1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry4 = (h4 + (i64) (1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry1 = (h1 + (i64) (1 << 24)) >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry5 = (h5 + (i64) (1 << 24)) >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;
    carry2 = (h2 + (i64) (1 << 25)) >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry6 = (h6 + (i64) (1 << 25)) >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;
    carry3 = (h3 + (i64) (1 << 24)) >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry7 = (h7 + (i64) (1 << 24)) >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;
    carry4 = (h4 + (i64) (1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry8 = (h8 + (i64) (1 << 25)) >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;
    carry9 = (h9 + (i64) (1 << 24)) >> 25;
    h0 += carry9 * 19;
    h9 -= carry9 << 25;
    carry0 = (h0 + (i64) (1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    h[0] = (i32) h0;
    h[1] = (i32) h1;
    h[2] = (i32) h2;
    h[3] = (i32) h3;
    h[4] = (i32) h4;
    h[5] = (i32) h5;
    h[6] = (i32) h6;
    h[7] = (i32) h7;
    h[8] = (i32) h8;
    h[9] = (i32) h9;
}


/*
h = 2 * f * f
Can overlap h with f.

Preconditions:
   |f| bounded by 1.65*2^26,1.65*2^25,1.65*2^26,1.65*2^25,etc.

Postconditions:
   |h| bounded by 1.01*2^25,1.01*2^24,1.01*2^25,1.01*2^24,etc.
*/

/*
See fe_mul.c for discussion of implementation strategy.
*/

void fe_sq2(fe h, const fe f) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i32 f0_2 = 2 * f0;
    i32 f1_2 = 2 * f1;
    i32 f2_2 = 2 * f2;
    i32 f3_2 = 2 * f3;
    i32 f4_2 = 2 * f4;
    i32 f5_2 = 2 * f5;
    i32 f6_2 = 2 * f6;
    i32 f7_2 = 2 * f7;
    i32 f5_38 = 38 * f5; /* 1.959375*2^30 */
    i32 f6_19 = 19 * f6; /* 1.959375*2^30 */
    i32 f7_38 = 38 * f7; /* 1.959375*2^30 */
    i32 f8_19 = 19 * f8; /* 1.959375*2^30 */
    i32 f9_38 = 38 * f9; /* 1.959375*2^30 */
    i64 f0f0    = f0   * (i64) f0;
    i64 f0f1_2  = f0_2 * (i64) f1;
    i64 f0f2_2  = f0_2 * (i64) f2;
    i64 f0f3_2  = f0_2 * (i64) f3;
    i64 f0f4_2  = f0_2 * (i64) f4;
    i64 f0f5_2  = f0_2 * (i64) f5;
    i64 f0f6_2  = f0_2 * (i64) f6;
    i64 f0f7_2  = f0_2 * (i64) f7;
    i64 f0f8_2  = f0_2 * (i64) f8;
    i64 f0f9_2  = f0_2 * (i64) f9;
    i64 f1f1_2  = f1_2 * (i64) f1;
    i64 f1f2_2  = f1_2 * (i64) f2;
    i64 f1f3_4  = f1_2 * (i64) f3_2;
    i64 f1f4_2  = f1_2 * (i64) f4;
    i64 f1f5_4  = f1_2 * (i64) f5_2;
    i64 f1f6_2  = f1_2 * (i64) f6;
    i64 f1f7_4  = f1_2 * (i64) f7_2;
    i64 f1f8_2  = f1_2 * (i64) f8;
    i64 f1f9_76 = f1_2 * (i64) f9_38;
    i64 f2f2    = f2   * (i64) f2;
    i64 f2f3_2  = f2_2 * (i64) f3;
    i64 f2f4_2  = f2_2 * (i64) f4;
    i64 f2f5_2  = f2_2 * (i64) f5;
    i64 f2f6_2  = f2_2 * (i64) f6;
    i64 f2f7_2  = f2_2 * (i64) f7;
    i64 f2f8_38 = f2_2 * (i64) f8_19;
    i64 f2f9_38 = f2   * (i64) f9_38;
    i64 f3f3_2  = f3_2 * (i64) f3;
    i64 f3f4_2  = f3_2 * (i64) f4;
    i64 f3f5_4  = f3_2 * (i64) f5_2;
    i64 f3f6_2  = f3_2 * (i64) f6;
    i64 f3f7_76 = f3_2 * (i64) f7_38;
    i64 f3f8_38 = f3_2 * (i64) f8_19;
    i64 f3f9_76 = f3_2 * (i64) f9_38;
    i64 f4f4    = f4   * (i64) f4;
    i64 f4f5_2  = f4_2 * (i64) f5;
    i64 f4f6_38 = f4_2 * (i64) f6_19;
    i64 f4f7_38 = f4   * (i64) f7_38;
    i64 f4f8_38 = f4_2 * (i64) f8_19;
    i64 f4f9_38 = f4   * (i64) f9_38;
    i64 f5f5_38 = f5   * (i64) f5_38;
    i64 f5f6_38 = f5_2 * (i64) f6_19;
    i64 f5f7_76 = f5_2 * (i64) f7_38;
    i64 f5f8_38 = f5_2 * (i64) f8_19;
    i64 f5f9_76 = f5_2 * (i64) f9_38;
    i64 f6f6_19 = f6   * (i64) f6_19;
    i64 f6f7_38 = f6   * (i64) f7_38;
    i64 f6f8_38 = f6_2 * (i64) f8_19;
    i64 f6f9_38 = f6   * (i64) f9_38;
    i64 f7f7_38 = f7   * (i64) f7_38;
    i64 f7f8_38 = f7_2 * (i64) f8_19;
    i64 f7f9_76 = f7_2 * (i64) f9_38;
    i64 f8f8_19 = f8   * (i64) f8_19;
    i64 f8f9_38 = f8   * (i64) f9_38;
    i64 f9f9_38 = f9   * (i64) f9_38;
    i64 h0 = f0f0  + f1f9_76 + f2f8_38 + f3f7_76 + f4f6_38 + f5f5_38;
    i64 h1 = f0f1_2 + f2f9_38 + f3f8_38 + f4f7_38 + f5f6_38;
    i64 h2 = f0f2_2 + f1f1_2 + f3f9_76 + f4f8_38 + f5f7_76 + f6f6_19;
    i64 h3 = f0f3_2 + f1f2_2 + f4f9_38 + f5f8_38 + f6f7_38;
    i64 h4 = f0f4_2 + f1f3_4 + f2f2   + f5f9_76 + f6f8_38 + f7f7_38;
    i64 h5 = f0f5_2 + f1f4_2 + f2f3_2 + f6f9_38 + f7f8_38;
    i64 h6 = f0f6_2 + f1f5_4 + f2f4_2 + f3f3_2 + f7f9_76 + f8f8_19;
    i64 h7 = f0f7_2 + f1f6_2 + f2f5_2 + f3f4_2 + f8f9_38;
    i64 h8 = f0f8_2 + f1f7_4 + f2f6_2 + f3f5_4 + f4f4   + f9f9_38;
    i64 h9 = f0f9_2 + f1f8_2 + f2f7_2 + f3f6_2 + f4f5_2;
    i64 carry0;
    i64 carry1;
    i64 carry2;
    i64 carry3;
    i64 carry4;
    i64 carry5;
    i64 carry6;
    i64 carry7;
    i64 carry8;
    i64 carry9;
    h0 += h0;
    h1 += h1;
    h2 += h2;
    h3 += h3;
    h4 += h4;
    h5 += h5;
    h6 += h6;
    h7 += h7;
    h8 += h8;
    h9 += h9;
    carry0 = (h0 + (i64) (1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry4 = (h4 + (i64) (1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry1 = (h1 + (i64) (1 << 24)) >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry5 = (h5 + (i64) (1 << 24)) >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;
    carry2 = (h2 + (i64) (1 << 25)) >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry6 = (h6 + (i64) (1 << 25)) >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;
    carry3 = (h3 + (i64) (1 << 24)) >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry7 = (h7 + (i64) (1 << 24)) >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;
    carry4 = (h4 + (i64) (1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry8 = (h8 + (i64) (1 << 25)) >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;
    carry9 = (h9 + (i64) (1 << 24)) >> 25;
    h0 += carry9 * 19;
    h9 -= carry9 << 25;
    carry0 = (h0 + (i64) (1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    h[0] = (i32) h0;
    h[1] = (i32) h1;
    h[2] = (i32) h2;
    h[3] = (i32) h3;
    h[4] = (i32) h4;
    h[5] = (i32) h5;
    h[6] = (i32) h6;
    h[7] = (i32) h7;
    h[8] = (i32) h8;
    h[9] = (i32) h9;
}


/*
h = f - g
Can overlap h with f or g.

Preconditions:
   |f| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.
   |g| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.

Postconditions:
   |h| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.
*/

void fe_sub(fe h, const fe f, const fe g) {
    i32 f0 = f[0];
    i32 f1 = f[1];
    i32 f2 = f[2];
    i32 f3 = f[3];
    i32 f4 = f[4];
    i32 f5 = f[5];
    i32 f6 = f[6];
    i32 f7 = f[7];
    i32 f8 = f[8];
    i32 f9 = f[9];
    i32 g0 = g[0];
    i32 g1 = g[1];
    i32 g2 = g[2];
    i32 g3 = g[3];
    i32 g4 = g[4];
    i32 g5 = g[5];
    i32 g6 = g[6];
    i32 g7 = g[7];
    i32 g8 = g[8];
    i32 g9 = g[9];
    i32 h0 = f0 - g0;
    i32 h1 = f1 - g1;
    i32 h2 = f2 - g2;
    i32 h3 = f3 - g3;
    i32 h4 = f4 - g4;
    i32 h5 = f5 - g5;
    i32 h6 = f6 - g6;
    i32 h7 = f7 - g7;
    i32 h8 = f8 - g8;
    i32 h9 = f9 - g9;

    h[0] = h0;
    h[1] = h1;
    h[2] = h2;
    h[3] = h3;
    h[4] = h4;
    h[5] = h5;
    h[6] = h6;
    h[7] = h7;
    h[8] = h8;
    h[9] = h9;
}



/*
Preconditions:
  |h| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.

Write p=2^255-19; q=floor(h/p).
Basic claim: q = floor(2^(-255)(h + 19 2^(-25)h9 + 2^(-1))).

Proof:
  Have |h|<=p so |q|<=1 so |19^2 2^(-255) q|<1/4.
  Also have |h-2^230 h9|<2^231 so |19 2^(-255)(h-2^230 h9)|<1/4.

  Write y=2^(-1)-19^2 2^(-255)q-19 2^(-255)(h-2^230 h9).
  Then 0<y<1.

  Write r=h-pq.
  Have 0<=r<=p-1=2^255-20.
  Thus 0<=r+19(2^-255)r<r+19(2^-255)2^255<=2^255-1.

  Write x=r+19(2^-255)r+y.
  Then 0<x<2^255 so floor(2^(-255)x) = 0 so floor(q+2^(-255)x) = q.

  Have q+2^(-255)x = 2^(-255)(h + 19 2^(-25) h9 + 2^(-1))
  so floor(2^(-255)(h + 19 2^(-25) h9 + 2^(-1))) = q.
*/

void fe_tobytes(u8 *s, const fe h) {
    i32 h0 = h[0];
    i32 h1 = h[1];
    i32 h2 = h[2];
    i32 h3 = h[3];
    i32 h4 = h[4];
    i32 h5 = h[5];
    i32 h6 = h[6];
    i32 h7 = h[7];
    i32 h8 = h[8];
    i32 h9 = h[9];
    i32 q;
    i32 carry0;
    i32 carry1;
    i32 carry2;
    i32 carry3;
    i32 carry4;
    i32 carry5;
    i32 carry6;
    i32 carry7;
    i32 carry8;
    i32 carry9;
    q = (19 * h9 + (((i32) 1) << 24)) >> 25;
    q = (h0 + q) >> 26;
    q = (h1 + q) >> 25;
    q = (h2 + q) >> 26;
    q = (h3 + q) >> 25;
    q = (h4 + q) >> 26;
    q = (h5 + q) >> 25;
    q = (h6 + q) >> 26;
    q = (h7 + q) >> 25;
    q = (h8 + q) >> 26;
    q = (h9 + q) >> 25;
    /* Goal: Output h-(2^255-19)q, which is between 0 and 2^255-20. */
    h0 += 19 * q;
    /* Goal: Output h-2^255 q, which is between 0 and 2^255-20. */
    carry0 = h0 >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry1 = h1 >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry2 = h2 >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry3 = h3 >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry4 = h4 >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry5 = h5 >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;
    carry6 = h6 >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;
    carry7 = h7 >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;
    carry8 = h8 >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;
    carry9 = h9 >> 25;
    h9 -= carry9 << 25;

    /* h10 = carry9 */
    /*
    Goal: Output h0+...+2^255 h10-2^255 q, which is between 0 and 2^255-20.
    Have h0+...+2^230 h9 between 0 and 2^255-1;
    evidently 2^255 h10-2^255 q = 0.
    Goal: Output h0+...+2^230 h9.
    */
    s[0] = (u8) (h0 >> 0);
    s[1] = (u8) (h0 >> 8);
    s[2] = (u8) (h0 >> 16);
    s[3] = (u8) ((h0 >> 24) | (h1 << 2));
    s[4] = (u8) (h1 >> 6);
    s[5] = (u8) (h1 >> 14);
    s[6] = (u8) ((h1 >> 22) | (h2 << 3));
    s[7] = (u8) (h2 >> 5);
    s[8] = (u8) (h2 >> 13);
    s[9] = (u8) ((h2 >> 21) | (h3 << 5));
    s[10] = (u8) (h3 >> 3);
    s[11] = (u8) (h3 >> 11);
    s[12] = (u8) ((h3 >> 19) | (h4 << 6));
    s[13] = (u8) (h4 >> 2);
    s[14] = (u8) (h4 >> 10);
    s[15] = (u8) (h4 >> 18);
    s[16] = (u8) (h5 >> 0);
    s[17] = (u8) (h5 >> 8);
    s[18] = (u8) (h5 >> 16);
    s[19] = (u8) ((h5 >> 24) | (h6 << 1));
    s[20] = (u8) (h6 >> 7);
    s[21] = (u8) (h6 >> 15);
    s[22] = (u8) ((h6 >> 23) | (h7 << 3));
    s[23] = (u8) (h7 >> 5);
    s[24] = (u8) (h7 >> 13);
    s[25] = (u8) ((h7 >> 21) | (h8 << 4));
    s[26] = (u8) (h8 >> 4);
    s[27] = (u8) (h8 >> 12);
    s[28] = (u8) ((h8 >> 20) | (h9 << 6));
    s[29] = (u8) (h9 >> 2);
    s[30] = (u8) (h9 >> 10);
    s[31] = (u8) (h9 >> 18);
}




/*
ge means group element.

Here the group is the set of pairs (x,y) of field elements (see fe.h)
satisfying -x^2 + y^2 = 1 + d x^2y^2
where d = -121665/121666.

Representations:
  ge_p2 (projective): (X:Y:Z) satisfying x=X/Z, y=Y/Z
  ge_p3 (extended): (X:Y:Z:T) satisfying x=X/Z, y=Y/Z, XY=ZT
  ge_p1p1 (completed): ((X:Z),(Y:T)) satisfying x=X/Z, y=Y/T
  ge_precomp (Duif): (y+x,y-x,2dxy)
*/

typedef struct {
  fe X;
  fe Y;
  fe Z;
} ge_p2;

typedef struct {
  fe X;
  fe Y;
  fe Z;
  fe T;
} ge_p3;

typedef struct {
  fe X;
  fe Y;
  fe Z;
  fe T;
} ge_p1p1;

typedef struct {
  fe yplusx;
  fe yminusx;
  fe xy2d;
} ge_precomp;

typedef struct {
  fe YplusX;
  fe YminusX;
  fe Z;
  fe T2d;
} ge_cached;

void ge_p3_tobytes(u8 *s, const ge_p3 *h);
void ge_tobytes(u8 *s, const ge_p2 *h);
int ge_frombytes_negate_vartime(ge_p3 *h, const u8 *s);

void ge_add(ge_p1p1 *r, const ge_p3 *p, const ge_cached *q);
void ge_sub(ge_p1p1 *r, const ge_p3 *p, const ge_cached *q);
void ge_double_scalarmult_vartime(ge_p2 *r, const u8 *a, const ge_p3 *A, const u8 *b);
void ge_madd(ge_p1p1 *r, const ge_p3 *p, const ge_precomp *q);
void ge_msub(ge_p1p1 *r, const ge_p3 *p, const ge_precomp *q);
void ge_scalarmult_base(ge_p3 *h, const u8 *a);

void ge_p1p1_to_p2(ge_p2 *r, const ge_p1p1 *p);
void ge_p1p1_to_p3(ge_p3 *r, const ge_p1p1 *p);
void ge_p2_0(ge_p2 *h);
void ge_p2_dbl(ge_p1p1 *r, const ge_p2 *p);
void ge_p3_0(ge_p3 *h);
void ge_p3_dbl(ge_p1p1 *r, const ge_p3 *p);
void ge_p3_to_cached(ge_cached *r, const ge_p3 *p);
void ge_p3_to_p2(ge_p2 *r, const ge_p3 *p);


// https://gist.github.com/irfansehic/9c0b204845370f372bdfccfb66ec5942
static const ge_precomp Bi[8] = {
    {
        { 25967493, -14356035, 29566456, 3660896, -12694345, 4014787, 27544626, -11754271, -6079156, 2047605 },
        { -12545711, 934262, -2722910, 3049990, -727428, 9406986, 12720692, 5043384, 19500929, -15469378 },
        { -8738181, 4489570, 9688441, -14785194, 10184609, -12363380, 29287919, 11864899, -24514362, -4438546 },
    },
    {
        { 15636291, -9688557, 24204773, -7912398, 616977, -16685262, 27787600, -14772189, 28944400, -1550024 },
        { 16568933, 4717097, -11556148, -1102322, 15682896, -11807043, 16354577, -11775962, 7689662, 11199574 },
        { 30464156, -5976125, -11779434, -15670865, 23220365, 15915852, 7512774, 10017326, -17749093, -9920357 },
    },
    {
        { 10861363, 11473154, 27284546, 1981175, -30064349, 12577861, 32867885, 14515107, -15438304, 10819380 },
        { 4708026, 6336745, 20377586, 9066809, -11272109, 6594696, -25653668, 12483688, -12668491, 5581306 },
        { 19563160, 16186464, -29386857, 4097519, 10237984, -4348115, 28542350, 13850243, -23678021, -15815942 },
    },
    {
        { 5153746, 9909285, 1723747, -2777874, 30523605, 5516873, 19480852, 5230134, -23952439, -15175766 },
        { -30269007, -3463509, 7665486, 10083793, 28475525, 1649722, 20654025, 16520125, 30598449, 7715701 },
        { 28881845, 14381568, 9657904, 3680757, -20181635, 7843316, -31400660, 1370708, 29794553, -1409300 },
    },
    {
        { -22518993, -6692182, 14201702, -8745502, -23510406, 8844726, 18474211, -1361450, -13062696, 13821877 },
        { -6455177, -7839871, 3374702, -4740862, -27098617, -10571707, 31655028, -7212327, 18853322, -14220951 },
        { 4566830, -12963868, -28974889, -12240689, -7602672, -2830569, -8514358, -10431137, 2207753, -3209784 },
    },
    {
        { -25154831, -4185821, 29681144, 7868801, -6854661, -9423865, -12437364, -663000, -31111463, -16132436 },
        { 25576264, -2703214, 7349804, -11814844, 16472782, 9300885, 3844789, 15725684, 171356, 6466918 },
        { 23103977, 13316479, 9739013, -16149481, 817875, -15038942, 8965339, -14088058, -30714912, 16193877 },
    },
    {
        { -33521811, 3180713, -2394130, 14003687, -16903474, -16270840, 17238398, 4729455, -18074513, 9256800 },
        { -25182317, -4174131, 32336398, 5036987, -21236817, 11360617, 22616405, 9761698, -19827198, 630305 },
        { -13720693, 2639453, -24237460, -7406481, 9494427, -5774029, -6554551, -15960994, -2449256, -14291300 },
    },
    {
        { -3151181, -5046075, 9282714, 6866145, -31907062, -863023, -18940575, 15033784, 25105118, -7894876 },
        { -24326370, 15950226, -31801215, -14592823, -11662737, -5090925, 1573892, -2625887, 2198790, -15804619 },
        { -3099351, 10324967, -2241613, 7453183, -5446979, -2735503, -13812022, -16236442, -32461234, -12290683 },
    },
};// didn't find full solution yet
static ge_precomp base[32][8];
static int active_row;
void cached_to_precomp(ge_precomp* preComp, ge_cached* cached)
{
    fe inverse;
    fe_invert(inverse, cached->Z);
    fe_mul(preComp->yminusx, cached->YminusX, inverse);
    fe_mul(preComp->yplusx, cached->YplusX, inverse);
    fe_mul(preComp->xy2d, cached->T2d, inverse);
}

void compute_row( ge_cached* b)
{
    ge_precomp result;
    ge_p3 p3;
    ge_p3_0(&p3);
    
    int j;
    for (  j = 0; j < 8; j++)
    {
        ge_p1p1 p1p1;
        ge_cached cached;

        ge_add(&p1p1, & p3, b);
        ge_p1p1_to_p3(& p3, & p1p1);
        ge_p3_to_cached(& cached, & p3);

        cached_to_precomp(&result, &cached);
        base[active_row][j] = result;
        // TODO WRITE TO FLASH  
        // int ap;
        // for(ap = 0; ap < 10; ap++)
        // {
        //     base[active_row][j][ap] = result.yplusx[ap];
        // }
        // for(ap = 0; ap < 10; ap++)
        // {
        //     base[active_row][j][ap] = result.yminusx[ap];
        // }        
        // for(ap = 0; ap < 10; ap++)
        // {
        //     base[active_row][j][ap] = result.xy2d[ap];
        // }
    }
}

void compute_lookup_table(ge_cached* b)
{
    ge_p3 p3;
    ge_p2 p2;
    ge_p1p1 p1p1;
    ge_cached cached;

    ge_p3_0(&p3);
    ge_add(&p1p1, &p3, b);
    ge_p1p1_to_p3(&p3, &p1p1);

    int i, k;
    for ( i = 0; i < 32; i++)
    {
        active_row = i;
        ge_p3_to_cached(&cached, &p3);
        
        compute_row(&cached);
        
        ge_p3_to_p2(&p2,  &p3);
        
        // Calculate next base point
        
        for ( k = 0; k < 7; k++)
        {
            ge_p2_dbl( &p1p1,  &p2);
            ge_p1p1_to_p2( &p2,  &p1p1);
        }
        ge_p2_dbl(& p1p1, & p2);
        ge_p1p1_to_p3(& p3, & p1p1);
    }
}

void LOOKT_write_lookup_table_to_flash(void)
{    
    // First Base point, B0
    fe ypx =  {25967493,-14356035,29566456,3660896,-12694345,4014787,27544626,-11754271,-6079156,2047605};
    fe ymx =  {-12545711,934262,-2722910,3049990,-727428,9406986,12720692,5043384,19500929,-15469378};
    fe T2d =  {-8738181,4489570,9688441,-14785194,10184609,-12363380,29287919,11864899,-24514362,-4438546};
    fe Z; fe_1(Z);
    
    ge_cached Bi0;
    
    int p;
    for(p = 0; p < 10; p++)
    {
        Bi0.YplusX[p] = ypx[p];
        Bi0.YminusX[p] = ymx[p];
        Bi0.T2d[p] = T2d[p];
        Bi0.Z[p] = Z[p];
    }
    
    compute_lookup_table(&Bi0);      
}


/*
r = p + q
*/

void ge_add(ge_p1p1 *r, const ge_p3 *p, const ge_cached *q) {
    fe t0;
    fe_add(r->X, p->Y, p->X);
    fe_sub(r->Y, p->Y, p->X);
    fe_mul(r->Z, r->X, q->YplusX);
    fe_mul(r->Y, r->Y, q->YminusX);
    fe_mul(r->T, q->T2d, p->T);
    fe_mul(r->X, p->Z, q->Z);
    fe_add(t0, r->X, r->X);
    fe_sub(r->X, r->Z, r->Y);
    fe_add(r->Y, r->Z, r->Y);
    fe_add(r->Z, t0, r->T);
    fe_sub(r->T, t0, r->T);
}


static void slide(signed char *r, const u8 *a) {
    int i;
    int b;
    int k;

    for (i = 0; i < 256; ++i) {
        r[i] = 1 & (a[i >> 3] >> (i & 7));
    }

    for (i = 0; i < 256; ++i)
        if (r[i]) {
            for (b = 1; b <= 6 && i + b < 256; ++b) {
                if (r[i + b]) {
                    if (r[i] + (r[i + b] << b) <= 15) {
                        r[i] += r[i + b] << b;
                        r[i + b] = 0;
                    } else if (r[i] - (r[i + b] << b) >= -15) {
                        r[i] -= r[i + b] << b;

                        for (k = i + b; k < 256; ++k) {
                            if (!r[k]) {
                                r[k] = 1;
                                break;
                            }

                            r[k] = 0;
                        }
                    } else {
                        break;
                    }
                }
            }
        }
}

/*
r = a * A + b * B
where a = a[0]+256*a[1]+...+256^31 a[31].
and b = b[0]+256*b[1]+...+256^31 b[31].
B is the Ed25519 base point (x,4/5) with x positive.
*/

void ge_double_scalarmult_vartime(ge_p2 *r, const u8 *a, const ge_p3 *A, const u8 *b) {
    signed char aslide[256];
    signed char bslide[256];
    ge_cached Ai[8]; /* A,3A,5A,7A,9A,11A,13A,15A */
    ge_p1p1 t;
    ge_p3 u;
    ge_p3 A2;
    int i;
    slide(aslide, a);
    slide(bslide, b);
    ge_p3_to_cached(&Ai[0], A);
    ge_p3_dbl(&t, A);
    ge_p1p1_to_p3(&A2, &t);
    ge_add(&t, &A2, &Ai[0]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[1], &u);
    ge_add(&t, &A2, &Ai[1]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[2], &u);
    ge_add(&t, &A2, &Ai[2]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[3], &u);
    ge_add(&t, &A2, &Ai[3]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[4], &u);
    ge_add(&t, &A2, &Ai[4]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[5], &u);
    ge_add(&t, &A2, &Ai[5]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[6], &u);
    ge_add(&t, &A2, &Ai[6]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[7], &u);
    ge_p2_0(r);

    for (i = 255; i >= 0; --i) {
        if (aslide[i] || bslide[i]) {
            break;
        }
    }

    for (; i >= 0; --i) {
        ge_p2_dbl(&t, r);

        if (aslide[i] > 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_add(&t, &u, &Ai[aslide[i] / 2]);
        } else if (aslide[i] < 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_sub(&t, &u, &Ai[(-aslide[i]) / 2]);
        }

        if (bslide[i] > 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_madd(&t, &u, &Bi[bslide[i] / 2]);
        } else if (bslide[i] < 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_msub(&t, &u, &Bi[(-bslide[i]) / 2]);
        }

        ge_p1p1_to_p2(r, &t);
    }
}


static const fe d = {
    -10913610, 13857413, -15372611, 6949391, 114729, -8787816, -6275908, -3247719, -18696448, -12055116
};

static const fe sqrtm1 = {
    -32595792, -7943725, 9377950, 3500415, 12389472, -272473, -25146209, -2005654, 326686, 11406482
};

int ge_frombytes_negate_vartime(ge_p3 *h, const u8 *s) {
    fe u;
    fe v;
    fe v3;
    fe vxx;
    fe check;
    fe_frombytes(h->Y, s);
    fe_1(h->Z);
    fe_sq(u, h->Y);
    fe_mul(v, u, d);
    fe_sub(u, u, h->Z);     /* u = y^2-1 */
    fe_add(v, v, h->Z);     /* v = dy^2+1 */
    fe_sq(v3, v);
    fe_mul(v3, v3, v);      /* v3 = v^3 */
    fe_sq(h->X, v3);
    fe_mul(h->X, h->X, v);
    fe_mul(h->X, h->X, u);  /* x = uv^7 */
    fe_pow22523(h->X, h->X); /* x = (uv^7)^((q-5)/8) */
    fe_mul(h->X, h->X, v3);
    fe_mul(h->X, h->X, u);  /* x = uv^3(uv^7)^((q-5)/8) */
    fe_sq(vxx, h->X);
    fe_mul(vxx, vxx, v);
    fe_sub(check, vxx, u);  /* vx^2-u */

    if (fe_isnonzero(check)) {
        fe_add(check, vxx, u); /* vx^2+u */

        if (fe_isnonzero(check)) {
            return -1;
        }

        fe_mul(h->X, h->X, sqrtm1);
    }

    if (fe_isnegative(h->X) == (s[31] >> 7)) {
        fe_neg(h->X, h->X);
    }

    fe_mul(h->T, h->X, h->Y);
    return 0;
}


/*
r = p + q
*/

void ge_madd(ge_p1p1 *r, const ge_p3 *p, const ge_precomp *q) {
    fe t0;
    fe_add(r->X, p->Y, p->X);
    fe_sub(r->Y, p->Y, p->X);
    fe_mul(r->Z, r->X, q->yplusx);
    fe_mul(r->Y, r->Y, q->yminusx);
    fe_mul(r->T, q->xy2d, p->T);
    fe_add(t0, p->Z, p->Z);
    fe_sub(r->X, r->Z, r->Y);
    fe_add(r->Y, r->Z, r->Y);
    fe_add(r->Z, t0, r->T);
    fe_sub(r->T, t0, r->T);
}


/*
r = p - q
*/

void ge_msub(ge_p1p1 *r, const ge_p3 *p, const ge_precomp *q) {
    fe t0;

    fe_add(r->X, p->Y, p->X);
    fe_sub(r->Y, p->Y, p->X);
    fe_mul(r->Z, r->X, q->yminusx);
    fe_mul(r->Y, r->Y, q->yplusx);
    fe_mul(r->T, q->xy2d, p->T);
    fe_add(t0, p->Z, p->Z);
    fe_sub(r->X, r->Z, r->Y);
    fe_add(r->Y, r->Z, r->Y);
    fe_sub(r->Z, t0, r->T);
    fe_add(r->T, t0, r->T);
}


/*
r = p
*/

void ge_p1p1_to_p2(ge_p2 *r, const ge_p1p1 *p) {
    fe_mul(r->X, p->X, p->T);
    fe_mul(r->Y, p->Y, p->Z);
    fe_mul(r->Z, p->Z, p->T);
}



/*
r = p
*/

void ge_p1p1_to_p3(ge_p3 *r, const ge_p1p1 *p) {
    fe_mul(r->X, p->X, p->T);
    fe_mul(r->Y, p->Y, p->Z);
    fe_mul(r->Z, p->Z, p->T);
    fe_mul(r->T, p->X, p->Y);
}


void ge_p2_0(ge_p2 *h) {
    fe_0(h->X);
    fe_1(h->Y);
    fe_1(h->Z);
}



/*
r = 2 * p
*/

void ge_p2_dbl(ge_p1p1 *r, const ge_p2 *p) {
    fe t0;

    fe_sq(r->X, p->X);
    fe_sq(r->Z, p->Y);
    fe_sq2(r->T, p->Z);
    fe_add(r->Y, p->X, p->Y);
    fe_sq(t0, r->Y);
    fe_add(r->Y, r->Z, r->X);
    fe_sub(r->Z, r->Z, r->X);
    fe_sub(r->X, t0, r->Y);
    fe_sub(r->T, r->T, r->Z);
}


void ge_p3_0(ge_p3 *h) {
    fe_0(h->X);
    fe_1(h->Y);
    fe_1(h->Z);
    fe_0(h->T);
}


/*
r = 2 * p
*/

void ge_p3_dbl(ge_p1p1 *r, const ge_p3 *p) {
    ge_p2 q;
    ge_p3_to_p2(&q, p);
    ge_p2_dbl(r, &q);
}



/*
r = p
*/

static const fe d2 = {
    -21827239, -5839606, -30745221, 13898782, 229458, 15978800, -12551817, -6495438, 29715968, 9444199
};

void ge_p3_to_cached(ge_cached *r, const ge_p3 *p) {
    fe_add(r->YplusX, p->Y, p->X);
    fe_sub(r->YminusX, p->Y, p->X);
    fe_copy(r->Z, p->Z);
    fe_mul(r->T2d, p->T, d2);
}


/*
r = p
*/

void ge_p3_to_p2(ge_p2 *r, const ge_p3 *p) {
    fe_copy(r->X, p->X);
    fe_copy(r->Y, p->Y);
    fe_copy(r->Z, p->Z);
}


void ge_p3_tobytes(u8 *s, const ge_p3 *h) {
    fe recip;
    fe x;
    fe y;
    fe_invert(recip, h->Z);
    fe_mul(x, h->X, recip);
    fe_mul(y, h->Y, recip);
    fe_tobytes(s, y);
    s[31] ^= fe_isnegative(x) << 7;
}


static u8 equal(signed char b, signed char c) {
    u8 ub = b;
    u8 uc = c;
    u8 x = ub ^ uc; /* 0: yes; 1..255: no */
    u64 y = x; /* 0: yes; 1..255: no */
    y -= 1; /* large: yes; 0..254: no */
    y >>= 63; /* 1: yes; 0: no */
    return (u8) y;
}

static u8 negative(signed char b) {
    u64 x = b; /* 18446744073709551361..18446744073709551615: yes; 0..255: no */
    x >>= 63; /* 1: yes; 0: no */
    return (u8) x;
}

static void cmov(ge_precomp *t, const ge_precomp *u, u8 b) {
    fe_cmov(t->yplusx, u->yplusx, b);
    fe_cmov(t->yminusx, u->yminusx, b);
    fe_cmov(t->xy2d, u->xy2d, b);
}


static void select(ge_precomp *t, int pos, signed char b) {
    ge_precomp minust;
    u8 bnegative = negative(b);
    u8 babs = b - (((-bnegative) & b) << 1);
    fe_1(t->yplusx);
    fe_1(t->yminusx);
    fe_0(t->xy2d);
    cmov(t, &base[pos][0], equal(babs, 1));
    cmov(t, &base[pos][1], equal(babs, 2));
    cmov(t, &base[pos][2], equal(babs, 3));
    cmov(t, &base[pos][3], equal(babs, 4));
    cmov(t, &base[pos][4], equal(babs, 5));
    cmov(t, &base[pos][5], equal(babs, 6));
    cmov(t, &base[pos][6], equal(babs, 7));
    cmov(t, &base[pos][7], equal(babs, 8));
    fe_copy(minust.yplusx, t->yminusx);
    fe_copy(minust.yminusx, t->yplusx);
    fe_neg(minust.xy2d, t->xy2d);
    cmov(t, &minust, bnegative);
}

/*
h = a * B
where a = a[0]+256*a[1]+...+256^31 a[31]
B is the Ed25519 base point (x,4/5) with x positive.

Preconditions:
  a[31] <= 127
*/

void ge_scalarmult_base(ge_p3 *h, const u8 *a) {
    signed char e[64];
    signed char carry;
    ge_p1p1 r;
    ge_p2 s;
    ge_precomp t;
    int i;

    for (i = 0; i < 32; ++i) {
        e[2 * i + 0] = (a[i] >> 0) & 15;
        e[2 * i + 1] = (a[i] >> 4) & 15;
    }

    /* each e[i] is between 0 and 15 */
    /* e[63] is between 0 and 7 */
    carry = 0;

    for (i = 0; i < 63; ++i) {
        e[i] += carry;
        carry = e[i] + 8;
        carry >>= 4;
        e[i] -= carry << 4;
    }

    e[63] += carry;
    /* each e[i] is between -8 and 8 */
    ge_p3_0(h);

    for (i = 1; i < 64; i += 2) {
        select(&t, i / 2, e[i]);
        ge_madd(&r, h, &t);
        ge_p1p1_to_p3(h, &r);
    }

    ge_p3_dbl(&r, h);
    ge_p1p1_to_p2(&s, &r);
    ge_p2_dbl(&r, &s);
    ge_p1p1_to_p2(&s, &r);
    ge_p2_dbl(&r, &s);
    ge_p1p1_to_p2(&s, &r);
    ge_p2_dbl(&r, &s);
    ge_p1p1_to_p3(h, &r);

    for (i = 0; i < 64; i += 2) {
        select(&t, i / 2, e[i]);
        ge_madd(&r, h, &t);
        ge_p1p1_to_p3(h, &r);
    }
}


/*
r = p - q
*/

void ge_sub(ge_p1p1 *r, const ge_p3 *p, const ge_cached *q) {
    fe t0;
    
    fe_add(r->X, p->Y, p->X);
    fe_sub(r->Y, p->Y, p->X);
    fe_mul(r->Z, r->X, q->YminusX);
    fe_mul(r->Y, r->Y, q->YplusX);
    fe_mul(r->T, q->T2d, p->T);
    fe_mul(r->X, p->Z, q->Z);
    fe_add(t0, r->X, r->X);
    fe_sub(r->X, r->Z, r->Y);
    fe_add(r->Y, r->Z, r->Y);
    fe_sub(r->Z, t0, r->T);
    fe_add(r->T, t0, r->T);
}


void ge_tobytes(u8 *s, const ge_p2 *h) {
    fe recip;
    fe x;
    fe y;
    fe_invert(recip, h->Z);
    fe_mul(x, h->X, recip);
    fe_mul(y, h->Y, recip);
    fe_tobytes(s, y);
    s[31] ^= fe_isnegative(x) << 7;
}


/*
The set of scalars is \Z/l
where l = 2^252 + 27742317777372353535851937790883648493.
*/

void sc_reduce(u8 *s);
void sc_muladd(u8 *s, const u8 *a, const u8 *b, const u8 *c);



// dupe with fe
// static u64 load_3(const u8 *in) {
//     u64 result;
// 
//     result = (u64) in[0];
//     result |= ((u64) in[1]) << 8;
//     result |= ((u64) in[2]) << 16;
// 
//     return result;
// }

// dupe with fe
// static u64 load_4(const u8 *in) {
//     u64 result;
// 
//     result = (u64) in[0];
//     result |= ((u64) in[1]) << 8;
//     result |= ((u64) in[2]) << 16;
//     result |= ((u64) in[3]) << 24;
//     
//     return result;
// }

/*
Input:
  s[0]+256*s[1]+...+256^63*s[63] = s

Output:
  s[0]+256*s[1]+...+256^31*s[31] = s mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
  Overwrites s in place.
*/

void sc_reduce(u8 *s) {
    i64 s0 = 2097151 & load_3(s);
    i64 s1 = 2097151 & (load_4(s + 2) >> 5);
    i64 s2 = 2097151 & (load_3(s + 5) >> 2);
    i64 s3 = 2097151 & (load_4(s + 7) >> 7);
    i64 s4 = 2097151 & (load_4(s + 10) >> 4);
    i64 s5 = 2097151 & (load_3(s + 13) >> 1);
    i64 s6 = 2097151 & (load_4(s + 15) >> 6);
    i64 s7 = 2097151 & (load_3(s + 18) >> 3);
    i64 s8 = 2097151 & load_3(s + 21);
    i64 s9 = 2097151 & (load_4(s + 23) >> 5);
    i64 s10 = 2097151 & (load_3(s + 26) >> 2);
    i64 s11 = 2097151 & (load_4(s + 28) >> 7);
    i64 s12 = 2097151 & (load_4(s + 31) >> 4);
    i64 s13 = 2097151 & (load_3(s + 34) >> 1);
    i64 s14 = 2097151 & (load_4(s + 36) >> 6);
    i64 s15 = 2097151 & (load_3(s + 39) >> 3);
    i64 s16 = 2097151 & load_3(s + 42);
    i64 s17 = 2097151 & (load_4(s + 44) >> 5);
    i64 s18 = 2097151 & (load_3(s + 47) >> 2);
    i64 s19 = 2097151 & (load_4(s + 49) >> 7);
    i64 s20 = 2097151 & (load_4(s + 52) >> 4);
    i64 s21 = 2097151 & (load_3(s + 55) >> 1);
    i64 s22 = 2097151 & (load_4(s + 57) >> 6);
    i64 s23 = (load_4(s + 60) >> 3);
    i64 carry0;
    i64 carry1;
    i64 carry2;
    i64 carry3;
    i64 carry4;
    i64 carry5;
    i64 carry6;
    i64 carry7;
    i64 carry8;
    i64 carry9;
    i64 carry10;
    i64 carry11;
    i64 carry12;
    i64 carry13;
    i64 carry14;
    i64 carry15;
    i64 carry16;

    s11 += s23 * 666643;
    s12 += s23 * 470296;
    s13 += s23 * 654183;
    s14 -= s23 * 997805;
    s15 += s23 * 136657;
    s16 -= s23 * 683901;
    s23 = 0;
    s10 += s22 * 666643;
    s11 += s22 * 470296;
    s12 += s22 * 654183;
    s13 -= s22 * 997805;
    s14 += s22 * 136657;
    s15 -= s22 * 683901;
    s22 = 0;
    s9 += s21 * 666643;
    s10 += s21 * 470296;
    s11 += s21 * 654183;
    s12 -= s21 * 997805;
    s13 += s21 * 136657;
    s14 -= s21 * 683901;
    s21 = 0;
    s8 += s20 * 666643;
    s9 += s20 * 470296;
    s10 += s20 * 654183;
    s11 -= s20 * 997805;
    s12 += s20 * 136657;
    s13 -= s20 * 683901;
    s20 = 0;
    s7 += s19 * 666643;
    s8 += s19 * 470296;
    s9 += s19 * 654183;
    s10 -= s19 * 997805;
    s11 += s19 * 136657;
    s12 -= s19 * 683901;
    s19 = 0;
    s6 += s18 * 666643;
    s7 += s18 * 470296;
    s8 += s18 * 654183;
    s9 -= s18 * 997805;
    s10 += s18 * 136657;
    s11 -= s18 * 683901;
    s18 = 0;
    carry6 = (s6 + (1 << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry8 = (s8 + (1 << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry10 = (s10 + (1 << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;
    carry12 = (s12 + (1 << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 << 21;
    carry14 = (s14 + (1 << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 << 21;
    carry16 = (s16 + (1 << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 << 21;
    carry7 = (s7 + (1 << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry9 = (s9 + (1 << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry11 = (s11 + (1 << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 << 21;
    carry13 = (s13 + (1 << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 << 21;
    carry15 = (s15 + (1 << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 << 21;
    s5 += s17 * 666643;
    s6 += s17 * 470296;
    s7 += s17 * 654183;
    s8 -= s17 * 997805;
    s9 += s17 * 136657;
    s10 -= s17 * 683901;
    s17 = 0;
    s4 += s16 * 666643;
    s5 += s16 * 470296;
    s6 += s16 * 654183;
    s7 -= s16 * 997805;
    s8 += s16 * 136657;
    s9 -= s16 * 683901;
    s16 = 0;
    s3 += s15 * 666643;
    s4 += s15 * 470296;
    s5 += s15 * 654183;
    s6 -= s15 * 997805;
    s7 += s15 * 136657;
    s8 -= s15 * 683901;
    s15 = 0;
    s2 += s14 * 666643;
    s3 += s14 * 470296;
    s4 += s14 * 654183;
    s5 -= s14 * 997805;
    s6 += s14 * 136657;
    s7 -= s14 * 683901;
    s14 = 0;
    s1 += s13 * 666643;
    s2 += s13 * 470296;
    s3 += s13 * 654183;
    s4 -= s13 * 997805;
    s5 += s13 * 136657;
    s6 -= s13 * 683901;
    s13 = 0;
    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;
    carry0 = (s0 + (1 << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 << 21;
    carry2 = (s2 + (1 << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 << 21;
    carry4 = (s4 + (1 << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 << 21;
    carry6 = (s6 + (1 << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry8 = (s8 + (1 << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry10 = (s10 + (1 << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;
    carry1 = (s1 + (1 << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 << 21;
    carry3 = (s3 + (1 << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 << 21;
    carry5 = (s5 + (1 << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 << 21;
    carry7 = (s7 + (1 << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry9 = (s9 + (1 << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry11 = (s11 + (1 << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 << 21;
    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;
    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 << 21;
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 << 21;
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 << 21;
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 << 21;
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 << 21;
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 << 21;
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;
    carry11 = s11 >> 21;
    s12 += carry11;
    s11 -= carry11 << 21;
    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;
    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 << 21;
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 << 21;
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 << 21;
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 << 21;
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 << 21;
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 << 21;
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;

    s[0] = (u8) (s0 >> 0);
    s[1] = (u8) (s0 >> 8);
    s[2] = (u8) ((s0 >> 16) | (s1 << 5));
    s[3] = (u8) (s1 >> 3);
    s[4] = (u8) (s1 >> 11);
    s[5] = (u8) ((s1 >> 19) | (s2 << 2));
    s[6] = (u8) (s2 >> 6);
    s[7] = (u8) ((s2 >> 14) | (s3 << 7));
    s[8] = (u8) (s3 >> 1);
    s[9] = (u8) (s3 >> 9);
    s[10] = (u8) ((s3 >> 17) | (s4 << 4));
    s[11] = (u8) (s4 >> 4);
    s[12] = (u8) (s4 >> 12);
    s[13] = (u8) ((s4 >> 20) | (s5 << 1));
    s[14] = (u8) (s5 >> 7);
    s[15] = (u8) ((s5 >> 15) | (s6 << 6));
    s[16] = (u8) (s6 >> 2);
    s[17] = (u8) (s6 >> 10);
    s[18] = (u8) ((s6 >> 18) | (s7 << 3));
    s[19] = (u8) (s7 >> 5);
    s[20] = (u8) (s7 >> 13);
    s[21] = (u8) (s8 >> 0);
    s[22] = (u8) (s8 >> 8);
    s[23] = (u8) ((s8 >> 16) | (s9 << 5));
    s[24] = (u8) (s9 >> 3);
    s[25] = (u8) (s9 >> 11);
    s[26] = (u8) ((s9 >> 19) | (s10 << 2));
    s[27] = (u8) (s10 >> 6);
    s[28] = (u8) ((s10 >> 14) | (s11 << 7));
    s[29] = (u8) (s11 >> 1);
    s[30] = (u8) (s11 >> 9);
    s[31] = (u8) (s11 >> 17);
}



/*
Input:
  a[0]+256*a[1]+...+256^31*a[31] = a
  b[0]+256*b[1]+...+256^31*b[31] = b
  c[0]+256*c[1]+...+256^31*c[31] = c

Output:
  s[0]+256*s[1]+...+256^31*s[31] = (ab+c) mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
*/

void sc_muladd(u8 *s, const u8 *a, const u8 *b, const u8 *c) {
    i64 a0 = 2097151 & load_3(a);
    i64 a1 = 2097151 & (load_4(a + 2) >> 5);
    i64 a2 = 2097151 & (load_3(a + 5) >> 2);
    i64 a3 = 2097151 & (load_4(a + 7) >> 7);
    i64 a4 = 2097151 & (load_4(a + 10) >> 4);
    i64 a5 = 2097151 & (load_3(a + 13) >> 1);
    i64 a6 = 2097151 & (load_4(a + 15) >> 6);
    i64 a7 = 2097151 & (load_3(a + 18) >> 3);
    i64 a8 = 2097151 & load_3(a + 21);
    i64 a9 = 2097151 & (load_4(a + 23) >> 5);
    i64 a10 = 2097151 & (load_3(a + 26) >> 2);
    i64 a11 = (load_4(a + 28) >> 7);
    i64 b0 = 2097151 & load_3(b);
    i64 b1 = 2097151 & (load_4(b + 2) >> 5);
    i64 b2 = 2097151 & (load_3(b + 5) >> 2);
    i64 b3 = 2097151 & (load_4(b + 7) >> 7);
    i64 b4 = 2097151 & (load_4(b + 10) >> 4);
    i64 b5 = 2097151 & (load_3(b + 13) >> 1);
    i64 b6 = 2097151 & (load_4(b + 15) >> 6);
    i64 b7 = 2097151 & (load_3(b + 18) >> 3);
    i64 b8 = 2097151 & load_3(b + 21);
    i64 b9 = 2097151 & (load_4(b + 23) >> 5);
    i64 b10 = 2097151 & (load_3(b + 26) >> 2);
    i64 b11 = (load_4(b + 28) >> 7);
    i64 c0 = 2097151 & load_3(c);
    i64 c1 = 2097151 & (load_4(c + 2) >> 5);
    i64 c2 = 2097151 & (load_3(c + 5) >> 2);
    i64 c3 = 2097151 & (load_4(c + 7) >> 7);
    i64 c4 = 2097151 & (load_4(c + 10) >> 4);
    i64 c5 = 2097151 & (load_3(c + 13) >> 1);
    i64 c6 = 2097151 & (load_4(c + 15) >> 6);
    i64 c7 = 2097151 & (load_3(c + 18) >> 3);
    i64 c8 = 2097151 & load_3(c + 21);
    i64 c9 = 2097151 & (load_4(c + 23) >> 5);
    i64 c10 = 2097151 & (load_3(c + 26) >> 2);
    i64 c11 = (load_4(c + 28) >> 7);
    i64 s0;
    i64 s1;
    i64 s2;
    i64 s3;
    i64 s4;
    i64 s5;
    i64 s6;
    i64 s7;
    i64 s8;
    i64 s9;
    i64 s10;
    i64 s11;
    i64 s12;
    i64 s13;
    i64 s14;
    i64 s15;
    i64 s16;
    i64 s17;
    i64 s18;
    i64 s19;
    i64 s20;
    i64 s21;
    i64 s22;
    i64 s23;
    i64 carry0;
    i64 carry1;
    i64 carry2;
    i64 carry3;
    i64 carry4;
    i64 carry5;
    i64 carry6;
    i64 carry7;
    i64 carry8;
    i64 carry9;
    i64 carry10;
    i64 carry11;
    i64 carry12;
    i64 carry13;
    i64 carry14;
    i64 carry15;
    i64 carry16;
    i64 carry17;
    i64 carry18;
    i64 carry19;
    i64 carry20;
    i64 carry21;
    i64 carry22;

    s0 = c0 + a0 * b0;
    s1 = c1 + a0 * b1 + a1 * b0;
    s2 = c2 + a0 * b2 + a1 * b1 + a2 * b0;
    s3 = c3 + a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
    s4 = c4 + a0 * b4 + a1 * b3 + a2 * b2 + a3 * b1 + a4 * b0;
    s5 = c5 + a0 * b5 + a1 * b4 + a2 * b3 + a3 * b2 + a4 * b1 + a5 * b0;
    s6 = c6 + a0 * b6 + a1 * b5 + a2 * b4 + a3 * b3 + a4 * b2 + a5 * b1 + a6 * b0;
    s7 = c7 + a0 * b7 + a1 * b6 + a2 * b5 + a3 * b4 + a4 * b3 + a5 * b2 + a6 * b1 + a7 * b0;
    s8 = c8 + a0 * b8 + a1 * b7 + a2 * b6 + a3 * b5 + a4 * b4 + a5 * b3 + a6 * b2 + a7 * b1 + a8 * b0;
    s9 = c9 + a0 * b9 + a1 * b8 + a2 * b7 + a3 * b6 + a4 * b5 + a5 * b4 + a6 * b3 + a7 * b2 + a8 * b1 + a9 * b0;
    s10 = c10 + a0 * b10 + a1 * b9 + a2 * b8 + a3 * b7 + a4 * b6 + a5 * b5 + a6 * b4 + a7 * b3 + a8 * b2 + a9 * b1 + a10 * b0;
    s11 = c11 + a0 * b11 + a1 * b10 + a2 * b9 + a3 * b8 + a4 * b7 + a5 * b6 + a6 * b5 + a7 * b4 + a8 * b3 + a9 * b2 + a10 * b1 + a11 * b0;
    s12 = a1 * b11 + a2 * b10 + a3 * b9 + a4 * b8 + a5 * b7 + a6 * b6 + a7 * b5 + a8 * b4 + a9 * b3 + a10 * b2 + a11 * b1;
    s13 = a2 * b11 + a3 * b10 + a4 * b9 + a5 * b8 + a6 * b7 + a7 * b6 + a8 * b5 + a9 * b4 + a10 * b3 + a11 * b2;
    s14 = a3 * b11 + a4 * b10 + a5 * b9 + a6 * b8 + a7 * b7 + a8 * b6 + a9 * b5 + a10 * b4 + a11 * b3;
    s15 = a4 * b11 + a5 * b10 + a6 * b9 + a7 * b8 + a8 * b7 + a9 * b6 + a10 * b5 + a11 * b4;
    s16 = a5 * b11 + a6 * b10 + a7 * b9 + a8 * b8 + a9 * b7 + a10 * b6 + a11 * b5;
    s17 = a6 * b11 + a7 * b10 + a8 * b9 + a9 * b8 + a10 * b7 + a11 * b6;
    s18 = a7 * b11 + a8 * b10 + a9 * b9 + a10 * b8 + a11 * b7;
    s19 = a8 * b11 + a9 * b10 + a10 * b9 + a11 * b8;
    s20 = a9 * b11 + a10 * b10 + a11 * b9;
    s21 = a10 * b11 + a11 * b10;
    s22 = a11 * b11;
    s23 = 0;
    carry0 = (s0 + (1 << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 << 21;
    carry2 = (s2 + (1 << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 << 21;
    carry4 = (s4 + (1 << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 << 21;
    carry6 = (s6 + (1 << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry8 = (s8 + (1 << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry10 = (s10 + (1 << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;
    carry12 = (s12 + (1 << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 << 21;
    carry14 = (s14 + (1 << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 << 21;
    carry16 = (s16 + (1 << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 << 21;
    carry18 = (s18 + (1 << 20)) >> 21;
    s19 += carry18;
    s18 -= carry18 << 21;
    carry20 = (s20 + (1 << 20)) >> 21;
    s21 += carry20;
    s20 -= carry20 << 21;
    carry22 = (s22 + (1 << 20)) >> 21;
    s23 += carry22;
    s22 -= carry22 << 21;
    carry1 = (s1 + (1 << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 << 21;
    carry3 = (s3 + (1 << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 << 21;
    carry5 = (s5 + (1 << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 << 21;
    carry7 = (s7 + (1 << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry9 = (s9 + (1 << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry11 = (s11 + (1 << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 << 21;
    carry13 = (s13 + (1 << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 << 21;
    carry15 = (s15 + (1 << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 << 21;
    carry17 = (s17 + (1 << 20)) >> 21;
    s18 += carry17;
    s17 -= carry17 << 21;
    carry19 = (s19 + (1 << 20)) >> 21;
    s20 += carry19;
    s19 -= carry19 << 21;
    carry21 = (s21 + (1 << 20)) >> 21;
    s22 += carry21;
    s21 -= carry21 << 21;
    s11 += s23 * 666643;
    s12 += s23 * 470296;
    s13 += s23 * 654183;
    s14 -= s23 * 997805;
    s15 += s23 * 136657;
    s16 -= s23 * 683901;
    s23 = 0;
    s10 += s22 * 666643;
    s11 += s22 * 470296;
    s12 += s22 * 654183;
    s13 -= s22 * 997805;
    s14 += s22 * 136657;
    s15 -= s22 * 683901;
    s22 = 0;
    s9 += s21 * 666643;
    s10 += s21 * 470296;
    s11 += s21 * 654183;
    s12 -= s21 * 997805;
    s13 += s21 * 136657;
    s14 -= s21 * 683901;
    s21 = 0;
    s8 += s20 * 666643;
    s9 += s20 * 470296;
    s10 += s20 * 654183;
    s11 -= s20 * 997805;
    s12 += s20 * 136657;
    s13 -= s20 * 683901;
    s20 = 0;
    s7 += s19 * 666643;
    s8 += s19 * 470296;
    s9 += s19 * 654183;
    s10 -= s19 * 997805;
    s11 += s19 * 136657;
    s12 -= s19 * 683901;
    s19 = 0;
    s6 += s18 * 666643;
    s7 += s18 * 470296;
    s8 += s18 * 654183;
    s9 -= s18 * 997805;
    s10 += s18 * 136657;
    s11 -= s18 * 683901;
    s18 = 0;
    carry6 = (s6 + (1 << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry8 = (s8 + (1 << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry10 = (s10 + (1 << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;
    carry12 = (s12 + (1 << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 << 21;
    carry14 = (s14 + (1 << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 << 21;
    carry16 = (s16 + (1 << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 << 21;
    carry7 = (s7 + (1 << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry9 = (s9 + (1 << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry11 = (s11 + (1 << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 << 21;
    carry13 = (s13 + (1 << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 << 21;
    carry15 = (s15 + (1 << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 << 21;
    s5 += s17 * 666643;
    s6 += s17 * 470296;
    s7 += s17 * 654183;
    s8 -= s17 * 997805;
    s9 += s17 * 136657;
    s10 -= s17 * 683901;
    s17 = 0;
    s4 += s16 * 666643;
    s5 += s16 * 470296;
    s6 += s16 * 654183;
    s7 -= s16 * 997805;
    s8 += s16 * 136657;
    s9 -= s16 * 683901;
    s16 = 0;
    s3 += s15 * 666643;
    s4 += s15 * 470296;
    s5 += s15 * 654183;
    s6 -= s15 * 997805;
    s7 += s15 * 136657;
    s8 -= s15 * 683901;
    s15 = 0;
    s2 += s14 * 666643;
    s3 += s14 * 470296;
    s4 += s14 * 654183;
    s5 -= s14 * 997805;
    s6 += s14 * 136657;
    s7 -= s14 * 683901;
    s14 = 0;
    s1 += s13 * 666643;
    s2 += s13 * 470296;
    s3 += s13 * 654183;
    s4 -= s13 * 997805;
    s5 += s13 * 136657;
    s6 -= s13 * 683901;
    s13 = 0;
    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;
    carry0 = (s0 + (1 << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 << 21;
    carry2 = (s2 + (1 << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 << 21;
    carry4 = (s4 + (1 << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 << 21;
    carry6 = (s6 + (1 << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry8 = (s8 + (1 << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry10 = (s10 + (1 << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;
    carry1 = (s1 + (1 << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 << 21;
    carry3 = (s3 + (1 << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 << 21;
    carry5 = (s5 + (1 << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 << 21;
    carry7 = (s7 + (1 << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry9 = (s9 + (1 << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry11 = (s11 + (1 << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 << 21;
    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;
    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 << 21;
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 << 21;
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 << 21;
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 << 21;
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 << 21;
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 << 21;
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;
    carry11 = s11 >> 21;
    s12 += carry11;
    s11 -= carry11 << 21;
    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;
    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 << 21;
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 << 21;
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 << 21;
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 << 21;
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 << 21;
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 << 21;
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 << 21;
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 << 21;
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 << 21;
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 << 21;
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 << 21;
    
    s[0] = (u8) (s0 >> 0);
    s[1] = (u8) (s0 >> 8);
    s[2] = (u8) ((s0 >> 16) | (s1 << 5));
    s[3] = (u8) (s1 >> 3);
    s[4] = (u8) (s1 >> 11);
    s[5] = (u8) ((s1 >> 19) | (s2 << 2));
    s[6] = (u8) (s2 >> 6);
    s[7] = (u8) ((s2 >> 14) | (s3 << 7));
    s[8] = (u8) (s3 >> 1);
    s[9] = (u8) (s3 >> 9);
    s[10] = (u8) ((s3 >> 17) | (s4 << 4));
    s[11] = (u8) (s4 >> 4);
    s[12] = (u8) (s4 >> 12);
    s[13] = (u8) ((s4 >> 20) | (s5 << 1));
    s[14] = (u8) (s5 >> 7);
    s[15] = (u8) ((s5 >> 15) | (s6 << 6));
    s[16] = (u8) (s6 >> 2);
    s[17] = (u8) (s6 >> 10);
    s[18] = (u8) ((s6 >> 18) | (s7 << 3));
    s[19] = (u8) (s7 >> 5);
    s[20] = (u8) (s7 >> 13);
    s[21] = (u8) (s8 >> 0);
    s[22] = (u8) (s8 >> 8);
    s[23] = (u8) ((s8 >> 16) | (s9 << 5));
    s[24] = (u8) (s9 >> 3);
    s[25] = (u8) (s9 >> 11);
    s[26] = (u8) ((s9 >> 19) | (s10 << 2));
    s[27] = (u8) (s10 >> 6);
    s[28] = (u8) ((s10 >> 14) | (s11 << 7));
    s[29] = (u8) (s11 >> 1);
    s[30] = (u8) (s11 >> 9);
    s[31] = (u8) (s11 >> 17);
}


#include <stddef.h>



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




void ed25519_create_keypair(u8 *public_key, u8 *private_key, const u8 *seed) {
    ge_p3 A;

    sha512(seed, 32, private_key);
    private_key[0] &= 248;
    private_key[31] &= 63;
    private_key[31] |= 64;

    ge_scalarmult_base(&A, private_key);
    ge_p3_tobytes(public_key, &A);
}




void ed25519_key_exchange(u8 *shared_secret, const u8 *public_key, const u8 *private_key) {
    u8 e[32];
    u32 i;
    
    fe x1;
    fe x2;
    fe z2;
    fe x3;
    fe z3;
    fe tmp0;
    fe tmp1;

    int pos;
    u32 swap;
    u32 b;

    /* copy the private key and make sure it's valid */
    for (i = 0; i < 32; ++i) {
        e[i] = private_key[i];
    }

    e[0] &= 248;
    e[31] &= 63;
    e[31] |= 64;

    /* unpack the public key and convert edwards to montgomery */
    /* due to CodesInChaos: montgomeryX = (edwardsY + 1)*inverse(1 - edwardsY) mod p */
    fe_frombytes(x1, public_key);
    fe_1(tmp1);
    fe_add(tmp0, x1, tmp1);
    fe_sub(tmp1, tmp1, x1);
    fe_invert(tmp1, tmp1);
    fe_mul(x1, tmp0, tmp1);

    fe_1(x2);
    fe_0(z2);
    fe_copy(x3, x1);
    fe_1(z3);

    swap = 0;
    for (pos = 254; pos >= 0; --pos) {
        b = e[pos / 8] >> (pos & 7);
        b &= 1;
        swap ^= b;
        fe_cswap(x2, x3, swap);
        fe_cswap(z2, z3, swap);
        swap = b;

        /* from montgomery.h */
        fe_sub(tmp0, x3, z3);
        fe_sub(tmp1, x2, z2);
        fe_add(x2, x2, z2);
        fe_add(z2, x3, z3);
        fe_mul(z3, tmp0, x2);
        fe_mul(z2, z2, tmp1);
        fe_sq(tmp0, tmp1);
        fe_sq(tmp1, x2);
        fe_add(x3, z3, z2);
        fe_sub(z2, z3, z2);
        fe_mul(x2, tmp1, tmp0);
        fe_sub(tmp1, tmp1, tmp0);
        fe_sq(z2, z2);
        fe_mul121666(z3, tmp1);
        fe_sq(x3, x3);
        fe_add(tmp0, tmp0, z3);
        fe_mul(z3, x1, z2);
        fe_mul(z2, tmp1, tmp0);
    }

    fe_cswap(x2, x3, swap);
    fe_cswap(z2, z3, swap);

    fe_invert(z2, z2);
    fe_mul(x2, x2, z2);
    fe_tobytes(shared_secret, x2);
}



#include <stdio.h>

int ed25519_create_seed(u8 *seed) {
    FILE *f = fopen("/dev/urandom", "rb");

    if (f == NULL) {
        return 1;
    }

    fread(seed, 1, 32, f);
    fclose(f);

    return 0;
}








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






static int consttime_equal(const u8 *x, const u8 *y) {
    u8 r = 0;

    r = x[0] ^ y[0];
    #define F(i) r |= x[i] ^ y[i]
    F(1);
    F(2);
    F(3);
    F(4);
    F(5);
    F(6);
    F(7);
    F(8);
    F(9);
    F(10);
    F(11);
    F(12);
    F(13);
    F(14);
    F(15);
    F(16);
    F(17);
    F(18);
    F(19);
    F(20);
    F(21);
    F(22);
    F(23);
    F(24);
    F(25);
    F(26);
    F(27);
    F(28);
    F(29);
    F(30);
    F(31);
    #undef F

    return !r;
}

int ed25519_verify(const u8 *signature, const u8 *message, size_t message_len, const u8 *public_key) {
    u8 h[64];
    u8 checker[32];
    sha512_context hash;
    ge_p3 A;
    ge_p2 R;

    if (signature[63] & 224) {
        return 0;
    }

    if (ge_frombytes_negate_vartime(&A, public_key) != 0) {
        return 0;
    }

    sha512_init(&hash);
    sha512_update(&hash, signature, 32);
    sha512_update(&hash, public_key, 32);
    sha512_update(&hash, message, message_len);
    sha512_final(&hash, h);
    
    sc_reduce(h);
    ge_double_scalarmult_vartime(&R, h, &A, signature + 32);
    ge_tobytes(checker, &R);

    if (!consttime_equal(checker, signature)) {
        return 0;
    }

    return 1;
}

/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */




/* the K array */
static const u64 K[80] = {
    UINT64_C(0x428a2f98d728ae22), UINT64_C(0x7137449123ef65cd), 
    UINT64_C(0xb5c0fbcfec4d3b2f), UINT64_C(0xe9b5dba58189dbbc),
    UINT64_C(0x3956c25bf348b538), UINT64_C(0x59f111f1b605d019), 
    UINT64_C(0x923f82a4af194f9b), UINT64_C(0xab1c5ed5da6d8118),
    UINT64_C(0xd807aa98a3030242), UINT64_C(0x12835b0145706fbe), 
    UINT64_C(0x243185be4ee4b28c), UINT64_C(0x550c7dc3d5ffb4e2),
    UINT64_C(0x72be5d74f27b896f), UINT64_C(0x80deb1fe3b1696b1), 
    UINT64_C(0x9bdc06a725c71235), UINT64_C(0xc19bf174cf692694),
    UINT64_C(0xe49b69c19ef14ad2), UINT64_C(0xefbe4786384f25e3), 
    UINT64_C(0x0fc19dc68b8cd5b5), UINT64_C(0x240ca1cc77ac9c65),
    UINT64_C(0x2de92c6f592b0275), UINT64_C(0x4a7484aa6ea6e483), 
    UINT64_C(0x5cb0a9dcbd41fbd4), UINT64_C(0x76f988da831153b5),
    UINT64_C(0x983e5152ee66dfab), UINT64_C(0xa831c66d2db43210), 
    UINT64_C(0xb00327c898fb213f), UINT64_C(0xbf597fc7beef0ee4),
    UINT64_C(0xc6e00bf33da88fc2), UINT64_C(0xd5a79147930aa725), 
    UINT64_C(0x06ca6351e003826f), UINT64_C(0x142929670a0e6e70),
    UINT64_C(0x27b70a8546d22ffc), UINT64_C(0x2e1b21385c26c926), 
    UINT64_C(0x4d2c6dfc5ac42aed), UINT64_C(0x53380d139d95b3df),
    UINT64_C(0x650a73548baf63de), UINT64_C(0x766a0abb3c77b2a8), 
    UINT64_C(0x81c2c92e47edaee6), UINT64_C(0x92722c851482353b),
    UINT64_C(0xa2bfe8a14cf10364), UINT64_C(0xa81a664bbc423001),
    UINT64_C(0xc24b8b70d0f89791), UINT64_C(0xc76c51a30654be30),
    UINT64_C(0xd192e819d6ef5218), UINT64_C(0xd69906245565a910), 
    UINT64_C(0xf40e35855771202a), UINT64_C(0x106aa07032bbd1b8),
    UINT64_C(0x19a4c116b8d2d0c8), UINT64_C(0x1e376c085141ab53), 
    UINT64_C(0x2748774cdf8eeb99), UINT64_C(0x34b0bcb5e19b48a8),
    UINT64_C(0x391c0cb3c5c95a63), UINT64_C(0x4ed8aa4ae3418acb), 
    UINT64_C(0x5b9cca4f7763e373), UINT64_C(0x682e6ff3d6b2b8a3),
    UINT64_C(0x748f82ee5defb2fc), UINT64_C(0x78a5636f43172f60), 
    UINT64_C(0x84c87814a1f0ab72), UINT64_C(0x8cc702081a6439ec),
    UINT64_C(0x90befffa23631e28), UINT64_C(0xa4506cebde82bde9), 
    UINT64_C(0xbef9a3f7b2c67915), UINT64_C(0xc67178f2e372532b),
    UINT64_C(0xca273eceea26619c), UINT64_C(0xd186b8c721c0c207), 
    UINT64_C(0xeada7dd6cde0eb1e), UINT64_C(0xf57d4f7fee6ed178),
    UINT64_C(0x06f067aa72176fba), UINT64_C(0x0a637dc5a2c898a6), 
    UINT64_C(0x113f9804bef90dae), UINT64_C(0x1b710b35131c471b),
    UINT64_C(0x28db77f523047d84), UINT64_C(0x32caab7b40c72493), 
    UINT64_C(0x3c9ebe0a15c9bebc), UINT64_C(0x431d67c49c100d4c),
    UINT64_C(0x4cc5d4becb3e42b6), UINT64_C(0x597f299cfc657e2a), 
    UINT64_C(0x5fcb6fab3ad6faec), UINT64_C(0x6c44198c4a475817)
};

/* Various logical functions */

#define ROR64c(x, y)     ( ((((x)&UINT64_C(0xFFFFFFFFFFFFFFFF))>>((u64)(y)&UINT64_C(63))) |       ((x)<<((u64)(64-((y)&UINT64_C(63)))))) & UINT64_C(0xFFFFFFFFFFFFFFFF))

#define STORE64H(x, y)                                                                        { (y)[0] = (u8)(((x)>>56)&255); (y)[1] = (u8)(((x)>>48)&255);          (y)[2] = (u8)(((x)>>40)&255); (y)[3] = (u8)(((x)>>32)&255);          (y)[4] = (u8)(((x)>>24)&255); (y)[5] = (u8)(((x)>>16)&255);          (y)[6] = (u8)(((x)>>8)&255); (y)[7] = (u8)((x)&255); }

#define LOAD64H(x, y)                                                         { x = (((u64)((y)[0] & 255))<<56)|(((u64)((y)[1] & 255))<<48) |          (((u64)((y)[2] & 255))<<40)|(((u64)((y)[3] & 255))<<32) |          (((u64)((y)[4] & 255))<<24)|(((u64)((y)[5] & 255))<<16) |          (((u64)((y)[6] & 255))<<8)|(((u64)((y)[7] & 255))); }


#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y)) 
#define S(x, n)         ROR64c(x, n)
#define R(x, n)         (((x) &UINT64_C(0xFFFFFFFFFFFFFFFF))>>((u64)n))
#define Sigma0(x)       (S(x, 28) ^ S(x, 34) ^ S(x, 39))
#define Sigma1(x)       (S(x, 14) ^ S(x, 18) ^ S(x, 41))
#define Gamma0(x)       (S(x, 1) ^ S(x, 8) ^ R(x, 7))
#define Gamma1(x)       (S(x, 19) ^ S(x, 61) ^ R(x, 6))
#ifndef MIN
   #define MIN(x, y) ( ((x)<(y))?(x):(y) )
#endif

/* compress 1024-bits */
static int sha512_compress(sha512_context *md, u8 *buf)
{
    u64 S[8], W[80], t0, t1;
    int i;

    /* copy state into S */
    for (i = 0; i < 8; i++) {
        S[i] = md->state[i];
    }

    /* copy the state into 1024-bits into W[0..15] */
    for (i = 0; i < 16; i++) {
        LOAD64H(W[i], buf + (8*i));
    }

    /* fill W[16..79] */
    for (i = 16; i < 80; i++) {
        W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
    }        

/* Compress */
    #define RND(a,b,c,d,e,f,g,h,i)     t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i];     t1 = Sigma0(a) + Maj(a, b, c);    d += t0;     h  = t0 + t1;

    for (i = 0; i < 80; i += 8) {
       RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i+0);
       RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],i+1);
       RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],i+2);
       RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],i+3);
       RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],i+4);
       RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],i+5);
       RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],i+6);
       RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],i+7);
   }

   #undef RND



    /* feedback */
   for (i = 0; i < 8; i++) {
        md->state[i] = md->state[i] + S[i];
    }

    return 0;
}


/**
   Initialize the hash state
   @param md   The hash state you wish to initialize
   @return 0 if successful
*/
int sha512_init(sha512_context * md) {
    if (md == NULL) return 1;

    md->curlen = 0;
    md->length = 0;
    md->state[0] = UINT64_C(0x6a09e667f3bcc908);
    md->state[1] = UINT64_C(0xbb67ae8584caa73b);
    md->state[2] = UINT64_C(0x3c6ef372fe94f82b);
    md->state[3] = UINT64_C(0xa54ff53a5f1d36f1);
    md->state[4] = UINT64_C(0x510e527fade682d1);
    md->state[5] = UINT64_C(0x9b05688c2b3e6c1f);
    md->state[6] = UINT64_C(0x1f83d9abfb41bd6b);
    md->state[7] = UINT64_C(0x5be0cd19137e2179);

    return 0;
}

/**
   Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
   @return 0 if successful
*/
int sha512_update (sha512_context * md, const u8 *in, size_t inlen)               
{                                                                                           
    size_t n;
    size_t i;                                                                        
    int           err;     
    if (md == NULL) return 1;  
    if (in == NULL) return 1;                                                              
    if (md->curlen > sizeof(md->buf)) {                             
       return 1;                                                            
    }                                                                                       
    while (inlen > 0) {                                                                     
        if (md->curlen == 0 && inlen >= 128) {                           
           if ((err = sha512_compress (md, (u8 *)in)) != 0) {               
              return err;                                                                   
           }                                                                                
           md->length += 128 * 8;                                        
           in             += 128;                                                    
           inlen          -= 128;                                                    
        } else {                                                                            
           n = MIN(inlen, (128 - md->curlen));

           for (i = 0; i < n; i++) {
            md->buf[i + md->curlen] = in[i];
           }


           md->curlen += n;                                                     
           in             += n;                                                             
           inlen          -= n;                                                             
           if (md->curlen == 128) {                                      
              if ((err = sha512_compress (md, md->buf)) != 0) {            
                 return err;                                                                
              }                                                                             
              md->length += 8*128;                                       
              md->curlen = 0;                                                   
           }                                                                                
       }                                                                                    
    }                                                                                       
    return 0;                                                                        
}

/**
   Terminate the hash to get the digest
   @param md  The hash state
   @param out [out] The destination of the hash (64 bytes)
   @return 0 if successful
*/
   int sha512_final(sha512_context * md, u8 *out)
   {
    int i;

    if (md == NULL) return 1;
    if (out == NULL) return 1;

    if (md->curlen >= sizeof(md->buf)) {
     return 1;
 }

    /* increase the length of the message */
 md->length += md->curlen * UINT64_C(8);

    /* append the '1' bit */
 md->buf[md->curlen++] = (u8)0x80;

    /* if the length is currently above 112 bytes we append zeros
     * then compress.  Then we can fall back to padding zeros and length
     * encoding like normal.
     */
     if (md->curlen > 112) {
        while (md->curlen < 128) {
            md->buf[md->curlen++] = (u8)0;
        }
        sha512_compress(md, md->buf);
        md->curlen = 0;
    }

    /* pad upto 120 bytes of zeroes 
     * note: that from 112 to 120 is the 64 MSB of the length.  We assume that you won't hash
     * > 2^64 bits of data... :-)
     */
while (md->curlen < 120) {
    md->buf[md->curlen++] = (u8)0;
}

    /* store length */
STORE64H(md->length, md->buf+120);
sha512_compress(md, md->buf);

    /* copy output */
for (i = 0; i < 8; i++) {
    STORE64H(md->state[i], out+(8*i));
}

return 0;
}

int sha512(const u8 *message, size_t message_len, u8 *out)
{
    sha512_context ctx;
    int ret;
    if ((ret = sha512_init(&ctx))) return ret;
    if ((ret = sha512_update(&ctx, message, message_len))) return ret;
    if ((ret = sha512_final(&ctx, out))) return ret;
    return 0;
}

// blockchain stuff
typedef u8 t_hash[64];
typedef u8 t_sign[64];
typedef u8 t_pub_key[32];
typedef u8 t_prv_key[64];

struct Block_header {
  u32       id;
  
  u32       version       ;
  t_hash    prev_hash     ;
  t_hash    merkle_tree   ;
  u32       issuer_addr   ;
  t_pub_key issuer_pub_key;
  u32       nonce         ;
  // end of block
  t_hash hash;
  t_sign sign;
};

struct Tx {
  u32     amount   ;
  u32     send_addr;
  u32     recv_addr;
  i32     bind_addr;
  t_sign  sign     ;
};

struct Block {
  struct Block_header header;
  vector<Tx> tx_list;
};

const u32 tx_fee = 1;
const u32 mining_reward = 1;

// single block



// 1024 block pack 



// mem state
struct MemState {
  // address_to_pub_key
  vector<t_pub_key> a2pk;
  // analog PascalCoin safebox
  vector<u32> balance;
  
} gms;
// global_mem_state



// SIGN_LEN
#define SL1   const int len = (                                                                             sizeof(u32      )+                                                                          sizeof(t_hash   )+                                                                          sizeof(t_hash   )+                                                                          sizeof(u32      )+                                                                          sizeof(t_pub_key)+                                                                          sizeof(u32      )                                                                         );                                                                                          u8 buffer[len];                                                                             u8 *buf_ptr = (u8*)&buffer;                                                                 memcpy(buf_ptr, &header.version        , sizeof(u32      ));buf_ptr+=sizeof(u32      );     memcpy(buf_ptr, &header.prev_hash      , sizeof(t_hash   ));buf_ptr+=sizeof(t_hash   );     memcpy(buf_ptr, &header.merkle_tree    , sizeof(t_hash   ));buf_ptr+=sizeof(t_hash   );     memcpy(buf_ptr, &header.issuer_addr    , sizeof(u32      ));buf_ptr+=sizeof(u32      );     memcpy(buf_ptr, &header.issuer_pub_key , sizeof(t_pub_key));buf_ptr+=sizeof(t_pub_key);     memcpy(buf_ptr, &header.nonce          , sizeof(u32      ));/*buf_ptr+=sizeof(u32      );*/   sha512_context ctx;                                                                         sha512_init(&ctx);                                                                          sha512_update(&ctx, (u8*)&buffer, len);                                                   

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
#define SL2   const int len = (                                                         sizeof(u32)+                                                            sizeof(u32)+                                                            sizeof(u32)+                                                            sizeof(i32)                                                           );                                                                      u8 buffer[len];                                                         u8 *buf_ptr = (u8*)&buffer;                                             memcpy(buf_ptr, &tx.amount    , sizeof(u32));buf_ptr+=sizeof(u32);      memcpy(buf_ptr, &tx.send_addr , sizeof(u32));buf_ptr+=sizeof(u32);      memcpy(buf_ptr, &tx.recv_addr , sizeof(u32));buf_ptr+=sizeof(u32);      memcpy(buf_ptr, &tx.bind_addr , sizeof(i32));/*buf_ptr+=sizeof(i32);*/  
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
// single block



// 1024 block pack 



// mem state
// TODO hardcode key pair
t_pub_key genesis_pub_key;
t_prv_key genesis_priv_key_FUCK_WE_CANT_SEND_FILE_IN_OUT_SOLUTION_TO_GENESIS_NODE_FUUUUUUUUUUUUUUUUUUU;

void gms_init() {
  // gms.a2pk.push_back(genesis_pub_key);
  // gms.balance.push_back(1e6);
}

class TemplateServer : public AbstractServer<TemplateServer> {
 public:
  TemplateServer(AbstractServerConnector &conn,
                 serverVersion_t type = JSONRPC_SERVER_V2)
      : AbstractServer<TemplateServer>(conn, type) {
    bindAndAddMethod(
        Procedure("balance", PARAMS_BY_NAME,
                           JSON_INTEGER, "address",
                           JSON_STRING, NULL),
        &TemplateServer::balanceI);
    bindAndAddMethod(
        Procedure(
            "transaction", PARAMS_BY_NAME, JSON_STRING,
            "amount", JSON_INTEGER, "from_address",
            JSON_STRING, "to_address", JSON_STRING, NULL),
        &TemplateServer::transactionI);
  }

  virtual void balanceI(const Value &request,
                               Value &response) {
    response = balance(request["address"].asString());
  }
  virtual void transactionI(const Value &request,
                            Value &response) {
    response = transaction( request["amount"].asInt(),
                            request["from_address"].asString(),
                            request["to_address"].asString());
  }
  virtual int balance(const string &address) = 0;
  virtual string transaction(int amount, const string &from_address,
                                  const string &to_address) = 0;
};

class MyServer : public TemplateServer {
 public:
  MyServer(AbstractServerConnector &connector,
               serverVersion_t type);

  int balance(const string &address) override;
  string transaction( int amount,
                      const string &from_address,
                      const string &to_address) override;
};

MyServer::MyServer(AbstractServerConnector &connector,
                           serverVersion_t type)
    : TemplateServer(connector, type) {}

int MyServer::balance(const string &address) {
  cout << "SERVER | Received in balance: address[" << address << "]"
            << endl;
  return 1;
}

string MyServer::transaction(int amount,
                            const string &from_address,
                            const string &to_address) {
  cout << "SERVER | Received in transaction: from_address[" << from_address
            << "], to_address[" << to_address << "], amount[" << amount << "]"
            << endl;
  return "transaction block hash";
}

int main() {
  LOOKT_write_lookup_table_to_flash();
  gms_init();
  
  unsigned char seed[32], public_key[32], private_key[64], signature[64];
  unsigned char other_public_key[32], other_private_key[64], shared_secret[32];
  const unsigned char message[] = "TEST MESSAGE";
  const int len = strlen((const char*)message);

  /* create a random seed, and a key pair out of that seed */
  if (ed25519_create_seed(seed)) {
      printf("error while generating seed\n");
      exit(1);
  }

  ed25519_create_keypair(public_key, private_key, seed);

  /* create signature on the message with the key pair */
  ed25519_sign(signature, message, len, public_key, private_key);

  /* verify the signature */
  if (ed25519_verify(signature, message, len, public_key)) {
      printf("valid signature\n");
  } else {
      printf("invalid signature\n");
  }

  /* create a dummy keypair to use for a key exchange, normally you'd only have
  the public key and receive it through some communication channel */
  if (ed25519_create_seed(seed)) {
      printf("error while generating seed\n");
      exit(1);
  }

  ed25519_create_keypair(other_public_key, other_private_key, seed);

  /* do a key exchange with other_public_key */
  ed25519_key_exchange(shared_secret, other_public_key, private_key);

  /* 
      the magic here is that ed25519_key_exchange(shared_secret, public_key,
      other_private_key); would result in the same shared_secret
  */
  // for(int i1=0;i1<32;i1++) {
  //   for(int i2=0;i2<8;i2++) {
  //     for(int i3=0;i3<10;i3++) {
  //       printf("%d ", base[i1][i2].yplusx[i3]);
  //     }
  //     printf("\n");
  //     for(int i3=0;i3<10;i3++) {
  //       printf("%d ", base[i1][i2].yminusx[i3]);
  //     }
  //     printf("\n");
  //     for(int i3=0;i3<10;i3++) {
  //       printf("%d ", base[i1][i2].xy2d[i3]);
  //     }
  //     printf("\n");
  //   }
  // }
  // HttpServer httpserver(10001);
  // MyServer s(httpserver, JSONRPC_SERVER_V1V2);
  // s.StartListening();
  // cout << "welcome to UTON HACK!" << endl;
  // this_thread::sleep_for(chrono::seconds(45));
  // s.StopListening();
  return 0;
}