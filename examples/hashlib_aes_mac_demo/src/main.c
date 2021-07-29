/*
 *--------------------------------------
 * Program Name:
 * Author:
 * License:
 * Description:
 *--------------------------------------
*/

#include <tice.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <hashlib.h>

#define CEMU_CONSOLE ((char*)0xFB0000)
uint8_t str[] = "The lazy fox jumped over the dog!";
#define KEYSIZE (256>>3)    // 256 bits
#define IV_LEN  AES_BLOCKSIZE   // defined <hashlib.h>

void hexdump(uint8_t *addr, size_t len, uint8_t *label){
    if(label) sprintf(CEMU_CONSOLE, "%s\n", label);
    else sprintf(CEMU_CONSOLE, "\n");
    for(size_t rem_len = len, ct=1; rem_len>0; rem_len--, addr++, ct++){
        sprintf(CEMU_CONSOLE, "%02X ", *addr);
        if(!(ct%AES_BLOCKSIZE)) sprintf(CEMU_CONSOLE, "\n");
    }
    sprintf(CEMU_CONSOLE, "\n");
}

int main(void)
{
    // reserve key schedule and key buffer, IV, and encrypt/decrypt buffers
    aes_ctx ctx_enc, ctx_mac;
    uint8_t key_aes[KEYSIZE];
    uint8_t key_mac[KEYSIZE];   // ***_MUST_*** use different keys
    uint8_t iv[IV_LEN];
    size_t str_len = strlen(str);
    size_t padded_len;
    size_t ct_len = hashlib_AESAuthCiphertextSize(str_len);
    
    uint8_t* ct = hashlib_AllocContext(ct_len);
    //uint8_t* rev_ct = hashlib_AllocContext(ct_len - (AES_BLOCKSIZE<<1));
    uint8_t* depad = hashlib_AllocContext(ct_len - (AES_BLOCKSIZE<<1));
    strcpy(CEMU_CONSOLE, "\n----- AES with MAC AUTH DEMO -----\n");
    memcpy(&ct[AES_BLOCKSIZE], str, str_len);
    padded_len = hashlib_AESPadMessage(&ct[AES_BLOCKSIZE], str_len, &ct[AES_BLOCKSIZE], SCHM_DEFAULT);
    hexdump(&ct[AES_BLOCKSIZE], padded_len, "-- Padded original msg --");
    
    
    // Load the distinct keys into respective key schedules
    hashlib_RandomBytes(key_aes, KEYSIZE);
    hashlib_RandomBytes(key_mac, KEYSIZE);
    hashlib_AESLoadKey(key_mac, &ctx_mac, (KEYSIZE<<3)); // requires size in bits, not bytes
    hashlib_AESLoadKey(key_aes, &ctx_enc, (KEYSIZE<<3)); // requires size in bits, not bytes
    
    // get random IV
    hashlib_RandomBytes(iv, IV_LEN);
    
    // call the function macro in the library header
    if(!hashlib_AESAuthEncrypt(&ct[AES_BLOCKSIZE], padded_len, ct, &ctx_enc, &ctx_mac, iv)){
		strcpy(CEMU_CONSOLE, "Encryption Failed for some reason.\n"); return 1;
	}
    
    // reverse the encryption
    hexdump(ct, ct_len, "-- Result for unmodified message --");
    if(hashlib_AESAuthDecrypt(ct, ct_len, ct, &ctx_enc, &ctx_mac))
        strcpy(CEMU_CONSOLE, "Decryption Succeeded, MAC Verified.\n");
    else {strcpy(CEMU_CONSOLE, "Decryption Failed, MAC mismatch.\n"); return 1;}
    
    hashlib_AESStripPadding(ct, padded_len, depad, SCHM_DEFAULT);
    hexdump(depad, padded_len, "-- Padded decrypted msg --");
    sprintf(CEMU_CONSOLE, "The string is '%s'.\n", depad);
    padded_len = hashlib_AESPadMessage(depad, str_len, &ct[AES_BLOCKSIZE], SCHM_DEFAULT);
    if(!hashlib_AESAuthEncrypt(&ct[AES_BLOCKSIZE], padded_len, ct, &ctx_enc, &ctx_mac, iv)){
		strcpy(CEMU_CONSOLE, "Encryption Failed for some reason.\n"); return 1;
	}
    // calculate the MAC of blocks0:end-1] of the decrypted msg
    // the output should match the MAC computed above
    strcpy(CEMU_CONSOLE, "-- Chosen Ciphertext Attack Demo --\n");
    strcpy(CEMU_CONSOLE, "Suppose attacker modifies a byte in the ciphertext:\nIn this case, the 12th byte after the IV. (So row 2, position 14 of the hexdump)\n");
    ct[AES_BLOCKSIZE+12] ^= 0x45;
    hexdump(ct, ct_len, NULL);
    strcpy(CEMU_CONSOLE, "Any key to continue\n");
    os_GetKey();
    if(hashlib_AESAuthDecrypt(ct, ct_len, ct, &ctx_enc, &ctx_mac))
        strcpy(CEMU_CONSOLE, "Decryption Succeeded, MAC Verified.\n");
    else {strcpy(CEMU_CONSOLE, "Decryption Failed, MAC mismatch.\n"); return 1;}
        
    strcpy(CEMU_CONSOLE, "\n");
    
    free(ct);
    //free(rev_ct);
    free(depad);
    return 0;
    
}
