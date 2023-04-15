
void add64iu(uint8_t *A, unsigned int B);
void zero64(uint8_t *A);
void powmod(uint8_t size, uint8_t *restrict base, uint24_t exp, const uint8_t *restrict mod);
void rmemcpy(void* dest, void* src, size_t len);
void aes_gf2_mul(uint8_t *op1, uint8_t *op2, uint8_t *out);

void ll_swap_bytes(uint8_t *ll);
void bytelen_to_bitlen(size_t val, uint8_t *dst);
