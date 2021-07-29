// Emulation for an Atmel AT93C86 Three-wire Serial EEPROM
/*
References:
http://ww1.microchip.com/downloads/en/DeviceDoc/21797L.pdf
https://ww1.microchip.com/downloads/en/DeviceDoc/21132F.pdf
https://www.jameco.com/Jameco/Products/ProdDS/1393660.pdf
https://www.etlweb.com/doc/at93c46.pdf
https://ww1.microchip.com/downloads/en/DeviceDoc/doc5140.pdf
https://github.com/neutronstriker/VUSB_93C46_PROG/blob/master/AT93c46.c
https://github.com/vtl/eeprom_93c86/blob/master/eeprom_93c86.cpp
https://github.com/mamedev/mame/blob/ee1e4f9683a4953cb9d88f9256017fcbc38e3144/src/devices/machine/eepromser.h
https://github.com/mamedev/mame/blob/ee1e4f9683a4953cb9d88f9256017fcbc38e3144/src/devices/machine/eepromser.cpp
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "at93c86.h"

#define EEPROM_DEFAULT_PATH "EEPROM.BIN"

#define BANK_SIZE  0x800
#define BIT_SELECT 0x01
#define BIT_CLOCK  0x02
#define BIT_DATA   0x04

#define STATE_CMD   0
#define STATE_VAR   1
#define STATE_READ  2
#define STATE_WRITE 3

#define CMD_PAGE  4
#define CMD_WRITE 5
#define CMD_READ  6

uint16_t eeprom_command		= 0;
uint16_t eeprom_var			= 0;
uint16_t eeprom_state		= 0;
uint16_t eeprom_data_read   = 0;
uint16_t eeprom_data_write  = 0;
uint16_t eeprom_bitcounter	= 0;
uint16_t eeprom_offset		= 0;
uint16_t eeprom_clock    	= 0;
static char file_path[1024];

static uint8_t device_initialized = 0;


void AT93C86_Init(char* eeprom_path){
    device_initialized = 1;
    strcpy(file_path,eeprom_path);
	FILE * fp = fopen(file_path, "r");
	if(!fp){
		fp = fopen(file_path, "w");
		uint8_t * content = (uint8_t *)malloc(BANK_SIZE);
		fwrite(content, BANK_SIZE, 1, fp);
		fclose(fp);
		free(content);
	} else {
		fclose(fp);
	}    
}

uint16_t DeviceOffset(){return (64 * sizeof(uint16_t) * eeprom_offset) + (sizeof(uint16_t) * eeprom_var);}

void DeviceWrite(uint16_t offset, uint16_t value){
	FILE * fp = fopen(file_path, "r+b");
	fseek(fp, offset, SEEK_SET);
	fwrite(&value, sizeof(uint16_t), 1, fp);
	fclose(fp);
}

uint16_t DeviceRead(uint16_t offset){
	uint16_t result;
	FILE * fp = fopen(file_path, "rb");
	fseek(fp, offset, SEEK_SET);
	fread(&result, sizeof(uint16_t), 1, fp);
	fclose(fp);
	return result;
}

void DeviceReset(){
	eeprom_state = STATE_CMD;
	eeprom_bitcounter = 0;
	eeprom_command = 0;
	eeprom_var = 0;
	eeprom_clock = 0;
	eeprom_data_read = 0;
	eeprom_data_write = 0;
}

void DeviceProcess(uint8_t bit){

	switch(eeprom_state){

		case STATE_CMD:

			eeprom_command |= bit << (3 - eeprom_bitcounter);
			eeprom_bitcounter++;

			if(eeprom_bitcounter >= 4)
			{
				eeprom_bitcounter = 0;
				eeprom_state = STATE_VAR;
			}
			break;

		case STATE_VAR:
		
			eeprom_var |= bit << (9 - eeprom_bitcounter);
			eeprom_bitcounter++;

			if(eeprom_bitcounter >= 10)
			{
				eeprom_bitcounter = 0;
				eeprom_state = (eeprom_command == CMD_READ ? STATE_READ : STATE_WRITE);
			}
			break;
			
		case STATE_WRITE:

			eeprom_data_write |= bit << (15 - eeprom_bitcounter);
			eeprom_bitcounter++;

			if(eeprom_bitcounter >= 16)
			{
				if(eeprom_command == CMD_PAGE)
				{
					if(eeprom_var == 0)
					{
						eeprom_offset++;
					} 
					else if(eeprom_var == 768)
					{
						eeprom_offset = 0;
					}
				}
				else if(eeprom_command == CMD_WRITE)
				{
					DeviceWrite(DeviceOffset(), eeprom_data_write);
				}
				DeviceReset();
			}
			break;

		case STATE_READ:

			if(eeprom_bitcounter == 0)
			{
				eeprom_data_read = DeviceRead(DeviceOffset());
			} else {
				eeprom_data_read <<= 1;
			}
			eeprom_bitcounter++;
			if(eeprom_bitcounter >= 17)
			{
				DeviceReset();
			}
			break;

	}


}

// Handlers
void AT93C86_HandleOutput(unsigned short value){
 
	if(!(value & BIT_SELECT)){
		DeviceReset();
	}
	else if(!(value & BIT_CLOCK)) 
	{
		eeprom_clock = 0;
	}
	else if(eeprom_clock)
	{
		return;
	} 
	else 
	{
		eeprom_clock = 1;
		DeviceProcess((value & BIT_DATA) ? 1 : 0);
	}
}

unsigned short AT93C86_HandleInput(){

	if(eeprom_state == STATE_READ){
		return (eeprom_data_read & 0x8000) ? 0xFFFF :  0xFFFE;
	}
	return 0xFFFF;
}

