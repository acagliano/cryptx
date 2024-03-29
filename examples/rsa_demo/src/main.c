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
#include <cryptx.h>

#define CEMU_CONSOLE ((char*)0xFB0000)
#define MODSIZE 256

void hexdump(uint8_t *addr, size_t len, uint8_t *label){
    if(label) sprintf(CEMU_CONSOLE, "\n%s\n", label);
    else sprintf(CEMU_CONSOLE, "\n");
    for(size_t rem_len = len, ct=1; rem_len>0; rem_len--, addr++, ct++){
        sprintf(CEMU_CONSOLE, "%02X ", *addr);
        if(!(ct%CRYPTX_AES_BLOCK_SIZE)) sprintf(CEMU_CONSOLE, "\n");
    }
    sprintf(CEMU_CONSOLE, "\n");
}

int main(void)
{
    uint8_t str[] = "The daring fox jumped over the dog.";
	uint8_t ciphertext[MODSIZE];
    uint8_t pubkey[MODSIZE];
	rsa_error_t error;
    
    // Always check for false return value from csrand_init()
    if(!cryptx_csrand_init()) return 1;
	
	// RSA public keys are not generated in this manner
	// you would generally receive said key from a remote host
	sprintf(CEMU_CONSOLE, "about to csrand\n");
    cryptx_csrand_fill(pubkey, MODSIZE);
    pubkey[MODSIZE-1] |= 1;
    
	sprintf(CEMU_CONSOLE, "\n\n----------------------------------\nHashlib RSA Demo\n");
	hexdump(str, strlen(str), "---Original String---");
	sprintf(CEMU_CONSOLE, "about to encrypt\n");
	error = cryptx_rsa_encrypt(str, strlen(str), pubkey, MODSIZE, ciphertext, SHA256);
	sprintf(CEMU_CONSOLE, "RSA encrypt done, exit code %u: \n", error);
	if(!error) hexdump(ciphertext, MODSIZE, "---RSA Encrypted---");
    return 0;
}
