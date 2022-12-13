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
#include <hashlib.h>

#include <encrypt.h>

#define CEMU_CONSOLE ((char*)0xFB0000)
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
	uint8_t secret1[CRYPTX_ECDH_PUBKEY_SIZE];		// privkey1 * pubkey2
	uint8_t secret2[CRYPTX_ECDH_PUBKEY_SIZE];		// privkey2 * pubkey1
	
	struct cryptx_ecdh_ctx test1, test2;
	ecdh_error_t err;
	
	// Always check for false return value from csrand_init()
	if(!cryptx_csrand_init(SAMPLING_FAST)) return 1;
	
	sprintf(CEMU_CONSOLE, "\n\n----------------------------------\nElliptic Curve Diffie-Hellman Demo\n");
	
	err = cryptx_ecdh_init(&test1);
	sprintf(CEMU_CONSOLE, "gen of keypair 1 complete, exit code:%u\n", err);
	if(!err){
		hexdump(test1.privkey, sizeof test1.privkey, "---keypair 1 private---");
		hexdump(test1.pubkey, sizeof test1.pubkey, "---keypair 1 public---");
	}
	
	
	err = cryptx_ecdh_init(&test2);
	sprintf(CEMU_CONSOLE, "gen of keypair 2 complete, exit code:%u\n", err);
	if(!err){
		hexdump(test2.privkey, sizeof test2.privkey, "---keypair 2 private---");
		hexdump(test2.pubkey, sizeof test2.pubkey, "---keypair 2 public---");
	}
	
	err = cryptx_ecdh_secret(&test1, test2.pubkey, secret1);
	sprintf(CEMU_CONSOLE, "gen of secret1 complete, exit code:%u\n", err);
	if(!err)
		hexdump(secret1, sizeof secret1, "---Secret 1 = Privkey1 * Pubkey2 * Cofactor---");
	
	err = cryptx_ecdh_secret(&test2, test1.pubkey, secret2);
	sprintf(CEMU_CONSOLE, "gen of secret2 complete, exit code:%u\n", err);
	if(!err)
		hexdump(secret2, sizeof secret2, "---Secret 2 = Privkey2 * Pubkey1 * Cofactor---");
	
	return 0;
}
