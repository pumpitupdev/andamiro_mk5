// LED Readout for Andamiro MK5
#include <stdint.h>
#include <stdio.h>

#include "sigdriver.h"

unsigned char sd_value = 0;

void SigDriver_HandleOutput(unsigned char val){
    sd_value = val;
}