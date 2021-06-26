#ifndef HASHLIB_H
#define HASHLIB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// INCLUDE FILE FOR HASHLIB

// ###################################################
// ## Context Definitions for Hashes and Encryption ##
// ###################################################

typedef struct _sha1_ctx {
	uint8_t data[64];
	uint32_t datalen;
	uint8_t bitlen[8];
	uint32_t state[5];
	uint32_t k[4];
} sha1_ctx;

typedef struct _sha256_ctx {
	uint8_t data[64];
	uint32_t datalen;
	uint8_t bitlen[8];
	uint32_t state[8];
} sha256_ctx;


typedef struct {
   uint32_t p[18];
   uint32_t s[4][256];
} blowfish_ctx;

typedef struct {
    uint24_t keysize;
    uint32_t round_keys[60];
} aes_ctx;


// ###############################
// ##### DEFINES and EQUATES #####
// ###############################

#define SHA1_DIGEST_LEN     20
#define SHA1_HEXSTR_LEN     (SHA1_DIGEST_LEN<<1) + 1        // twice the digest, plus null terminator

#define SHA256_DIGEST_LEN   32
#define SHA256_HEXSTR_LEN   (SHA256_DIGEST_LEN<<1) + 1      // twice the digest, plus null terminator

#define AES_BLOCKSIZE 16


/*
    Erases the data in a context, ensuring that no traces of cryptographic arithmetic remain.
    
    # Inputs #
    <> ctx = pointer to an arbitrary context type from this library
    <> len = length in bytes to zero
    
    Example: hashlib_EraseContext(&sha1_ctx, sizeof(sha1_ctx));
    * It is advised to call this on every context declared in your program before exiting or freeing that region
 */
void hashlib_EraseContext(void *ctx, size_t len);

/*
    Pads input data to a cipher based on cipher data according to a selected padding scheme
    
    # Inputs #
    <> plaintext = pointer to buffer containing data to pad
    <> len = size of data to pad, in bytes (real size, not block-aligned size)
    <> outbuf = pointer to buffer large enough to hold padded data (see macros below)
    <> alg = encryption algorithm to pad for (see enumerations below)
    <> schm = padding scheme to pad with (see enumerations below)
 */
size_t hashlib_PadInputPlaintext(
    const uint8_t* plaintext,
    size_t len,
    uint8_t* outbuf,
    uint8_t alg,
    uint8_t schm);
    
enum _enc_algs {
    ALG_RSA,
    ALG_AES
};

enum _padding_schemes {
    SCHM_DEFAULT,
    SCHM_PKCS7,         // Pad with padding size        |   (AES)   *Default*
    SCHM_ISO_M2,        // Pad with 0x80,0x00...0x00    |   (AES)
    SCHM_ISO_M1,        // Pad with 0x00...0x00         |   (AES)
    SCHM_ANSIX923,      // Pad with randomness          |   (AES)
    SCHM_RSA_OAEP       // OAEP encoding                |   (RSA)   *Default*
};

// Macros to Return Correct Padding Size

#define hashlib_GetAESPaddedSize(len)  ((((len)%2)==0) ? len + AES_BLOCKSIZE : ((len/AES_BLOCKSIZE) + 1) * AES_BLOCKSIZE)

#define hashlib_GetRSAPaddedSize(len)   ((len)+36)

/*
#################################################
### Crypto-Safe Psuedorandom Number Generator ###
#################################################

    This PRNG utilizes an internal state that is controlled by the lib.
    While users can read from it and add entropy to it manually, they cannot modify it
        directly.
    The CSPRNG consists of a single `volatile uint8_t*`, internally called `eread` as well as
        a 128-byte entropy pool.
   
    ### Polling for Most Entropic Bit ###
        * hashlib_CSPRNGInit();
        <> For each byte in the memory region affected by bus noise, read each bit 1024 times
        <> If the bit was set, increment a counter
        <> Set initial minimum deviation to +/- 1/4 of the read count from absolute 50/50 split.
            For example, out of 1024 tests, a bit is considered to be of sufficient entropy
            if it deviates from 256-768 1's, with values closer to 512 being preferred.
        <> Select the byte with the bit that deviates from 512 the least.
    
    ** hashlib_CSPRNGInit() will return NULL if it was unable to find a bit of sufficient
        entropy on your device. If this happens for you, let me know. I will have you run a program
        I wrote to dump the stats from your calc's bus and send me the 8xv it generates. We will
        update the candidate table accordingly. **
        
    ### Generation of Random Numbers ###
        * hashlib_CSPRNGRandom()
        <> Allocate but do not assign a uint32_t. This will fill the value with garbage.
        <> Read from eread 4 times, XOR'ing each read with 8 bits of our partial rand.
        <> SHA-256 hash the entropy pool, then XOR the hash with the partial rand in 4-byte blocks.
        <> Call hashlib_CSPRNGAddEntropy() to scramble the state
        <> Return the uint32_t
        
    * hashlib_CSPRNGAddEntropy() may be called by the user in a loop to provide more dynamic entropy
        than this lib can provide by default
        
    * This CSPRNG passes all dieharder tests for a sample size of 16,384 bytes *
*/


//  Initialize the cryptographic RNG by finding the byte of most entropy on the current device
//  Returns the address selected as a (void*)
void* hashlib_CSPRNGInit(void);

//  Reads from byte selected 128 times, XORing new reads with existing data in the entropy pool
bool hashlib_CSPRNGAddEntropy(void);
 
//  Returns a 32-bit integer, derived from the entropy pool
uint32_t hashlib_CSPRNGRandom(void);


/*
    Fills a buffer to (size) with random bytes using the internal CSPRNG
    
    # Inputs #
    <> buffer = buffer to fill with random data
    <> size = how many bytes to write
    
    # Outputs #
    <> True if no errors encountered
    <> False if buffer was NULL or size was 0
    ** It is planned to make this compatible with beck's BIGINT lib
 */
bool hashlib_RandomBytes(uint8_t *buffer, size_t size);



// ##################
// ### SHA-1 HASH ###
// ##################

/*
    Init Context for SHA-1
        
    # Inputs #
    <> ctx = pointer to an SHA1_CTX
    ** SHA-1 will be invalid if this function is not called before hashing
    ** contexts are specific to a hash-stream. If there is another block of data you
        want to hash concurrently, you will need to init a new context
*/
void hashlib_Sha1Init(sha1_ctx *ctx);

/*
    Update Context for SHA-1

    # Inputs #
    <> ctx = pointer to an SHA1_CTX
    <> buf = ptr to a block of data to hash
    <> len = size of the block of data to hash
    ** Remember, if hashlib_Sha1Init is not called first, your hash will be wrong
*/
void hashlib_Sha1Update(sha1_ctx *ctx, const uint8_t *buf, size_t len);

/*
    Finalize Context and Render Digest for SHA-1

    # Inputs #
    <> ctx = pointer to an SHA1_CTX
    <> digest = pointer to buffer to write digest
*/
void hashlib_Sha1Final(sha1_ctx *ctx, uint8_t *digest);

/*
    One-Shot SHA-1 Computation

    # Inputs #
    <> buf = pointer to data to hash
    <> len = length of data to hash
    <> digest = pointer to buffer to write digest
*/
static inline void hashlib_SHA1(uint8_t *buf, size_t len, uint8_t *digest) {
	sha1_ctx ctx;
	hashlib_sha1init(&ctx);
	hashlib_sha1update(&ctx, buf, len);
	hashlib_sha1final(&ctx, digest);
}

// ####################
// ### SHA-256 HASH ###
// ####################

/*
    Init Context for SHA-256

    # Inputs #
    <> ctx = pointer to an SHA256_CTX
    ** SHA-256 will be invalid if this function is not called before hashing
    ** contexts are specific to a hash-stream. If there is another block of data you
        want to hash concurrently, you will need to init a new context
*/
void hashlib_Sha256Init(sha256_ctx *ctx);

/*
    Update Context for SHA-256

    # Inputs #
    <> ctx = pointer to an SHA256_CTX
    <> buf = ptr to a block of data to hash
    <> len = size of the block of data to hash
    ** Remember, if hashlib_Sha256Init is not called first, your hash will be wrong
*/
void hashlib_Sha256Update(sha256_ctx *ctx, const uint8_t *buf, uint32_t len);

/*
    Finalize Context and Render Digest for SHA-256

    # Inputs #
    <> ctx = pointer to an SHA256_CTX
    <> digest = pointer to buffer to write digest
*/
void hashlib_Sha256Final(sha256_ctx *ctx, uint8_t *digest);

/*
    One-Shot SHA-256 Computation

    # Inputs #
    <> buf = pointer to data to hash
    <> len = length of data to hash
    <> digest = pointer to buffer to write digest
*/
inline void hashlib_SHA256(uint8_t *buf, size_t len, uint8_t *digest) {
	sha256_ctx ctx;
	hashlib_sha256init(&ctx);
	hashlib_sha256update(&ctx, buf, len);
	hashlib_sha256final(&ctx, digest);
}

// ##########################################
// ### ADVANCED ENCRYPTION STANDARD (AES) ###
// ##########################################
// 32-bit implementation
// 128, 192, or 256 bit keys
// 10, 12, or 14 rounds
// Cipher Block Chaining Mode only
        
/*
    AES Import Key
    ** The key can be a user-generated byte sequence, ranging from 32 bits to 448 bits
    
    # Inputs #
    <> key = pointer to a 128, 192, or 256 bit key
    <> ks = pointer to an AES key schedule context
    <> size = size of the key, in bits
     */
void hashlib_AESLoadKey(const uint8_t* key, aes_ctx* ks, size_t keysize);

/*
    AES-CBC Encrypt
    
    # Inputs #
    <> plaintext = pointer to data to encrypt (pass through padding helper func first)
    <> len = size of data to encrypt
    <> ciphertext = pointer to buffer to write encrypted output
    <> ks = pointer to ks schedule initialized with AESLoadKey
    <> iv = pointer to initialization vector (psuedorandom 16-byte field)
 */
bool hashlib_AESEncrypt(
    const uint8_t* plaintext,
    size_t len,
    uint8_t* ciphertext,
    aes_ctx* ks,
    const uint8_t* iv);
    
/*
    <> ciphertext = pointer to data to decrypt
    <> len = size of data to decrypt
    <> plaintext = pointer to buffer to write decompressed output
    <> ks = pointer to initialized key schedule
    <> iv = pointer to initialization vector 
 */
size_t hashlib_AESDecrypt(
    const uint8_t* ciphertext,
    size_t len,
    uint8_t* plaintext,
    aes_ctx* ks,
    const uint8_t* iv);


// ###############################
// #### BASE 64 ENCODE/DECODE ####
// ###############################
// Helper functions for bcrypt, but may be useful

/*
    Encode a byte string into a base-64 string
    
    # Inputs #
    <> b64buffer = pointer to a buffer to write base-64 output
    <> data = pointer to buffer containing data to encode
    <> len = length of data to encode
 */
bool hashlib_b64encode(char *b64buffer, const uint8_t *data, size_t len);

/*
    Decode a base-64 string
    
    # Inputs #
    <> buffer = pointer to buffer to write decoded output
    <> len = size of data to output (this is output length, not input length)
    <> b64data = pointer to buffer containing data to decode
 */
bool hashlib_b64decode(uint8_t *buffer, size_t len, const char *b64data);

#endif