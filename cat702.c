// Emulation for a CAT702 ZNSEC DIP20 Lockchip
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cat702.h"

#define BIT_SELECT 0x10
#define BIT_CLOCK  0x20
#define BIT_DATA   0x40

static unsigned char PROGRAMMED_KEY[8] = {0x00};
#ifndef BYTE 
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef VOID
#define VOID void
#endif

BYTE chip_state;
BYTE chip_bit;

WORD port_current;
WORD port_old;

VOID piusec_start()
{
	chip_state = 0xAB;
	chip_bit = 0;
}

VOID piusec_apply_initial_sbox()
{
	static const unsigned char initial_sbox[8] = { 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x7f };

	BYTE r = 0;

	for (BYTE i = 0; i < 8; i++)
	{
		if (chip_state & (1 << i))
			r ^= initial_sbox[i];
	}

	chip_state = r;
}

BYTE piusec_compute_sbox_coef(BYTE sel, BYTE bit)
{
	if (!sel)
		return PROGRAMMED_KEY[bit];

	BYTE r = piusec_compute_sbox_coef((sel - 1) & 7, (bit - 1) & 7);
	r = (r << 1) | (((r >> 7) ^ (r >> 6)) & 1);

	if (bit != 7)
		return r;

	return r ^ piusec_compute_sbox_coef(sel, 0);
}

VOID piusec_apply_bit_sbox()
{
	BYTE r = 0;
	for (BYTE i = 0; i < 8; i++)
	{
		if (chip_state & (1 << i))
		{
			r ^= piusec_compute_sbox_coef(chip_bit, i);
		}
	}
	chip_state = r;
}

BYTE piusec_read()
{
	return (chip_state >> chip_bit) & 1;
}

VOID piusec_step(BYTE input)
{
	if ((input & 1) == 0)
	{
		piusec_apply_bit_sbox();
	}

	chip_bit++;
	chip_bit &= 7;

	if (chip_bit == 0)
	{
		piusec_apply_initial_sbox();
	}
}

void CAT702_HandleOutput(unsigned short value){
	port_old = port_current;
	port_current = value;

	if ((port_current ^ port_old) & BIT_SELECT)
	{
		piusec_start();
	}

	if ((port_current & BIT_CLOCK)
		&& !(port_old & BIT_CLOCK))
	{
		piusec_step(port_current & BIT_DATA ? 1 : 0);
	}
}

unsigned short CAT702_HandleInput(){
  return piusec_read() ? 1 : 0;
}


void CAT702_Initialize_Key(unsigned char* key_data){
	memcpy(PROGRAMMED_KEY,key_data,8);
}
