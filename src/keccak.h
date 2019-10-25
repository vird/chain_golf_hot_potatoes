/* 64 bitwise rotation to left */
#define ROTL64(x, y) (((x)<<(y))|((x)>>(64-(y))))

typedef struct {
	int b, l, w, nr;
} keccak_t;

void compute_rho(int w);

int keccakf(int, u64*);
int keccak(int r, int c, int n, int l, u8* M, u8* O);

int sha3_512(u8* M, int l, u8* O);
int sha3_384(u8* M, int l, u8* O);
int sha3_256(u8* M, int l, u8* O);
int sha3_224(u8* M, int l, u8* O);
