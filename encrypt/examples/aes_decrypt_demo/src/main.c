/*
 *--------------------------------------
 * Program Name:
 * Author:
 * License:
 * Description:
 *--------------------------------------
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <encrypt.h>

#define CEMU_CONSOLE ((char*)0xFB0000)
char *str1 = "The lazy fox jumped over the dog!";
char *str2 = "The lazier fox fell down!";
#define KEYSIZE (256>>3)    // 256 bits converted to bytes

// change to #define CBC_MODE to test CBC mode instead
#define CTR_MODE

void hexdump(uint8_t *addr, size_t len, char *label){
    if(label) sprintf(CEMU_CONSOLE, "\n%s\n", label);
    else sprintf(CEMU_CONSOLE, "\n");
    for(size_t rem_len = len, ct=1; rem_len>0; rem_len--, addr++, ct++){
        sprintf(CEMU_CONSOLE, "\\x%02X", *addr);
        if(!(ct%AES_BLOCKSIZE)) sprintf(CEMU_CONSOLE, "\n");
    }
    sprintf(CEMU_CONSOLE, "\n");
}

int main(void)
{
    // reserve key schedule and key buffer, IV.
    aes_ctx ctx;
    aes_error_t error;
    
    uint8_t buf1[256];
    uint8_t buf2[256];
    uint8_t stripped[256];
    
    sprintf(CEMU_CONSOLE, "\n---------------------------\nHASHLIB AES Decrypt Demo and Test\n");
    #ifdef CBC_MODE
    sprintf(CEMU_CONSOLE, "\n----- CBC Mode -----\n");
    #endif
    #ifdef CTR_MODE
    sprintf(CEMU_CONSOLE, "\n----- CTR Mode -----\n");
    #endif
    
    // generate random key and IV
    if(!csrand_init()) return 1;          // <<<----- DONT FORGET THIS
    // !!!! NEVER PROCEED WITH ANYTHING CRYPTOGRAPHIC !!!!
    // !!!! IF THE CSRNG FAILS TO INIT !!!!
    
   // use same key/iv as in aes_test.py for testing
   uint8_t key[] = {0xEE,0x89,0x19,0xC3,0x8D,0x53,0x7A,0xD6,0x04,0x19,0x9E,0x77,0x0B,0xE0,0xE0,0x4C,0x4C,0x70,0xDB,0xE1,0x22,0x79,0xE1,0x90,0x06,0x1B,0xAF,0x99,0x49,0x8E,0x66,0x73};
   uint8_t iv[] = {0x79,0xA6,0xDE,0xDF,0xF0,0xA2,0x7C,0x7F,0xEE,0x0B,0x8E,0xF5,0x12,0x63,0xA4,0x8A};
   
   uint8_t ct1[33] = {0x4e,0xf7,0x4f,0xae,0x2d,0xe8,0x77,0x7d,0xd9,0x1e,0xf6,0x3c,0xb6,0x71,0x98,0x20,0x33,0xd9,0xe5,0x30,0x31,0xb8,0xc1,0x3c,0x2e,0x38,0x36,0xa6,0x81,0x6b,0xba,0x46,0x20};
   uint8_t ct2[25] = {0x27,0xdc,0x0e,0x7a,0x57,0x48,0xe0,0x72,0x78,0x33,0xa9,0xfe,0x99,0x13,0x12,0xcf,0xe6,0xba,0x4c,0xf5,0x59,0x98,0x5c,0x88,0xf8};
    
    // show the IV and key for testing purposes
    hexdump(key, KEYSIZE, "-- AES secret --");
    hexdump(iv, AES_IVSIZE, "-- initialization vector --");
    
    // initialize the AES key schedule and set cipher mode
    #ifdef CBC_MODE
    aes_init(&ctx, key, KEYSIZE, iv, AES_MODE_CBC);
    #endif
    #ifdef CTR_MODE
    aes_init(&ctx, key, KEYSIZE, iv, AES_MODE_CTR);
    #endif
    sprintf(CEMU_CONSOLE, "init complete\n");
    
    // decrypt message 1 and output return code and decrypted data for testing
    error = aes_decrypt(&ctx, ct1, sizeof ct1, buf1);
	sprintf(CEMU_CONSOLE, "message segment 1 done, exit code %u\n", error);
    
    // decrypt message 2 and output return code and decrypted data for testing
    error = aes_decrypt(&ctx, ct2, sizeof ct2, buf2);
    sprintf(CEMU_CONSOLE, "message segment 2 done, exit code %u\n", error);
    
    hexdump(buf1, sizeof ct1, "-- Decrypted Message 1 --");
    hexdump(buf2, sizeof ct2, "-- Decrypted Message 2 --");
    
    return 0;
    
}