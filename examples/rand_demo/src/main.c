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
#include <tice.h>
#include <cryptx.h>

#define CEMU_CONSOLE ((char*)0xFB0000)
#define SALT_LEN 16     // for example, a salt that is an IV for AES is 16 bytes long

int main(void)
{
    // reserve key schedule and key buffer, IV, and encrypt/decrypt buffers
    uint8_t salt[SALT_LEN];

    // initialize the secure RNG. Do not forget this!!!
    if(!cryptx_csrand_init()) return 1;
	sprintf(CEMU_CONSOLE, "RNG init done.\n");
	
	// generate a single uint32_t
	sprintf(CEMU_CONSOLE, "The rand is %lu.\n", cryptx_csrand_get());
    
    // or fill a buffer to size with random bytes
    cryptx_csrand_fill(salt, SALT_LEN);
    strcpy(CEMU_CONSOLE, "The buffer contents are: \n");
    for(uint8_t i=0; i<SALT_LEN; i++)
        sprintf(CEMU_CONSOLE, "%02X ", salt[i]);
    
	strcpy(CEMU_CONSOLE, "\n");
    return 0;
    
}
