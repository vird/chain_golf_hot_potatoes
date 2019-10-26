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

#include "fixedint.h"
#include "sha512.h"
#define Y UINT64_C

/* the K array */
static const u64 K[80] = {
    Y(0x428a2f98d728ae22), Y(0x7137449123ef65cd), 
    Y(0xb5c0fbcfec4d3b2f), Y(0xe9b5dba58189dbbc),
    Y(0x3956c25bf348b538), Y(0x59f111f1b605d019), 
    Y(0x923f82a4af194f9b), Y(0xab1c5ed5da6d8118),
    Y(0xd807aa98a3030242), Y(0x12835b0145706fbe), 
    Y(0x243185be4ee4b28c), Y(0x550c7dc3d5ffb4e2),
    Y(0x72be5d74f27b896f), Y(0x80deb1fe3b1696b1), 
    Y(0x9bdc06a725c71235), Y(0xc19bf174cf692694),
    Y(0xe49b69c19ef14ad2), Y(0xefbe4786384f25e3), 
    Y(0x0fc19dc68b8cd5b5), Y(0x240ca1cc77ac9c65),
    Y(0x2de92c6f592b0275), Y(0x4a7484aa6ea6e483), 
    Y(0x5cb0a9dcbd41fbd4), Y(0x76f988da831153b5),
    Y(0x983e5152ee66dfab), Y(0xa831c66d2db43210), 
    Y(0xb00327c898fb213f), Y(0xbf597fc7beef0ee4),
    Y(0xc6e00bf33da88fc2), Y(0xd5a79147930aa725), 
    Y(0x06ca6351e003826f), Y(0x142929670a0e6e70),
    Y(0x27b70a8546d22ffc), Y(0x2e1b21385c26c926), 
    Y(0x4d2c6dfc5ac42aed), Y(0x53380d139d95b3df),
    Y(0x650a73548baf63de), Y(0x766a0abb3c77b2a8), 
    Y(0x81c2c92e47edaee6), Y(0x92722c851482353b),
    Y(0xa2bfe8a14cf10364), Y(0xa81a664bbc423001),
    Y(0xc24b8b70d0f89791), Y(0xc76c51a30654be30),
    Y(0xd192e819d6ef5218), Y(0xd69906245565a910), 
    Y(0xf40e35855771202a), Y(0x106aa07032bbd1b8),
    Y(0x19a4c116b8d2d0c8), Y(0x1e376c085141ab53), 
    Y(0x2748774cdf8eeb99), Y(0x34b0bcb5e19b48a8),
    Y(0x391c0cb3c5c95a63), Y(0x4ed8aa4ae3418acb), 
    Y(0x5b9cca4f7763e373), Y(0x682e6ff3d6b2b8a3),
    Y(0x748f82ee5defb2fc), Y(0x78a5636f43172f60), 
    Y(0x84c87814a1f0ab72), Y(0x8cc702081a6439ec),
    Y(0x90befffa23631e28), Y(0xa4506cebde82bde9), 
    Y(0xbef9a3f7b2c67915), Y(0xc67178f2e372532b),
    Y(0xca273eceea26619c), Y(0xd186b8c721c0c207), 
    Y(0xeada7dd6cde0eb1e), Y(0xf57d4f7fee6ed178),
    Y(0x06f067aa72176fba), Y(0x0a637dc5a2c898a6), 
    Y(0x113f9804bef90dae), Y(0x1b710b35131c471b),
    Y(0x28db77f523047d84), Y(0x32caab7b40c72493), 
    Y(0x3c9ebe0a15c9bebc), Y(0x431d67c49c100d4c),
    Y(0x4cc5d4becb3e42b6), Y(0x597f299cfc657e2a), 
    Y(0x5fcb6fab3ad6faec), Y(0x6c44198c4a475817)
};

/* Various logical functions */

#define ROR64c(x, y) \
    ( ((((x)&Y(0xFFFFFFFFFFFFFFFF))>>((u64)(y)&Y(63))) | \
      ((x)<<((u64)(64-((y)&Y(63)))))) & Y(0xFFFFFFFFFFFFFFFF))

#define STORE64H(x, y)                                                                     \
   { (y)[0] = (u8)(((x)>>56)&255); (y)[1] = (u8)(((x)>>48)&255);     \
     (y)[2] = (u8)(((x)>>40)&255); (y)[3] = (u8)(((x)>>32)&255);     \
     (y)[4] = (u8)(((x)>>24)&255); (y)[5] = (u8)(((x)>>16)&255);     \
     (y)[6] = (u8)(((x)>>8)&255); (y)[7] = (u8)((x)&255); }

#define LOAD64H(x, y)                                                      \
   { x = (((u64)((y)[0] & 255))<<56)|(((u64)((y)[1] & 255))<<48) | \
         (((u64)((y)[2] & 255))<<40)|(((u64)((y)[3] & 255))<<32) | \
         (((u64)((y)[4] & 255))<<24)|(((u64)((y)[5] & 255))<<16) | \
         (((u64)((y)[6] & 255))<<8)|(((u64)((y)[7] & 255))); }


#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y)) 
#define S(x, n)         ROR64c(x, n)
#define R(x, n)         (((x) &Y(0xFFFFFFFFFFFFFFFF))>>((u64)n))
#define Sigma0(x)       (S(x, 28) ^ S(x, 34) ^ S(x, 39))
#define Sigma1(x)       (S(x, 14) ^ S(x, 18) ^ S(x, 41))
#define Gamma0(x)       (S(x, 1) ^ S(x, 8) ^ R(x, 7))
#define Gamma1(x)       (S(x, 19) ^ S(x, 61) ^ R(x, 6))

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
    #define RND(a,b,c,d,e,f,g,h,i) \
    t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i]; \
    t1 = Sigma0(a) + Maj(a, b, c);\
    d += t0; \
    h  = t0 + t1;

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
    md->state[0] = Y(0x6a09e667f3bcc908);
    md->state[1] = Y(0xbb67ae8584caa73b);
    md->state[2] = Y(0x3c6ef372fe94f82b);
    md->state[3] = Y(0xa54ff53a5f1d36f1);
    md->state[4] = Y(0x510e527fade682d1);
    md->state[5] = Y(0x9b05688c2b3e6c1f);
    md->state[6] = Y(0x1f83d9abfb41bd6b);
    md->state[7] = Y(0x5be0cd19137e2179);

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
           n = min(inlen, (128 - md->curlen));

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
 md->length += md->curlen * Y(8);

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
