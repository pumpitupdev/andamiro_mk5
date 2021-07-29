// Andamiro MK5 Baseboard Module
#include <stdio.h>
#include <stdlib.h>
#include "cat702.h"
#include "am030614.h"
#include "keyboardio.h"
#include "at93c86.h"
#include "ioport.h"

#include "mk5.h"

#if defined _WIN32
#define EXPORTABLE __declspec(dllexport)
EXPORTABLE void andamiro_mk5(){}
#endif

void __attribute__((constructor)) MK5_Init();

static int initialized = 0;
static unsigned char HexChar(char c) {
	if ('0' <= c && c <= '9') return (unsigned char)(c - '0');
	if ('A' <= c && c <= 'F') return (unsigned char)(c - 'A' + 10);
	if ('a' <= c && c <= 'f') return (unsigned char)(c - 'a' + 10);
	return 0xFF;
}

static int HexToBin(const char* s, unsigned char* buff, int length) {
	int result;
	if (!s || !buff || length <= 0) return -1;

	for (result = 0; *s; ++result)
	{
		unsigned char msn = HexChar(*s++);
		if (msn == 0xFF) return -1;
		unsigned char lsn = HexChar(*s++);
		if (lsn == 0xFF) return -1;
		unsigned char bin = (msn << 4) + lsn;

		if (length-- <= 0) return -1;
		*buff++ = bin;
	}
	return result;
}

void MK5_Init(){
    if(initialized){return;}
    // Initialize AM030614 [MK5 PIUIO]
    PIUIO_Init();
    if(getenv("PIUIO_KEY")){
        KeyboardIO_Init();
    }

    // Initialize AT83C86 EEPROM
    char* eeprom_path = getenv("EEPROM_PATH");
    eeprom_path ? AT93C86_Init(eeprom_path) : AT93C86_Init("EEPROM.BIN");
    
    // Initialize CAT702 Lockchip Module
    char* cat702_key_s = getenv("CAT702_KEY");
    if(cat702_key_s){
        unsigned char key_data[8] = {0x00};
        HexToBin(cat702_key_s,key_data,8);
        CAT702_Initialize_Key(key_data);
    }

    // Initialize IOPort Bindings
    IOPort_Init();   
    initialized = 1;
}


