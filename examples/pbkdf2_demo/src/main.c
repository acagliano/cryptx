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
#define RECD_MIN_SALT_LEN   16      // recommended minimum salt length
uint8_t salt[RECD_MIN_SALT_LEN] = {
    0xea,0x53,0xad,0xb5,0x34,0x96,0xdc,0xdd,
    0xd9,0xd8,0xf1,0x50,0x4c,0x9d,0xfb,0x4d};   // 16 bytes
uint8_t passwd[] = "testing123";
#define HEXDUMP_LINE_LEN	16

void hexdump(uint8_t *addr, size_t len, uint8_t *label){
    if(label) sprintf(CEMU_CONSOLE, "\n%s\n", label);
    else sprintf(CEMU_CONSOLE, "\n");
    for(size_t rem_len = len, ct=1; rem_len>0; rem_len--, addr++, ct++){
        sprintf(CEMU_CONSOLE, "%02X ", *addr);
		if(!(ct%HEXDUMP_LINE_LEN)) sprintf(CEMU_CONSOLE, "\n");
    }
    sprintf(CEMU_CONSOLE, "\n");
}

int main(void)
{
	char outbuf[64];
    size_t str_len = strlen(passwd);
    cryptx_hmac_pbkdf2(passwd, str_len, salt, RECD_MIN_SALT_LEN, outbuf, 64, 10, SHA256);
    hexdump(outbuf, sizeof outbuf, "-PBKDF2 output-");
    strcpy(CEMU_CONSOLE, "\n");
}
