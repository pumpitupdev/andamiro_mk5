// Emulation for Andamiro AM030614 MK5 Pump IO Board
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "am030614.h"

#define FP_LEFTUP		0x0001
#define FP_RIGHTUP		0x0002
#define FP_CENTER		0x0004
#define FP_LEFTDOWN		0x0008
#define FP_RIGHTDOWN	0x0010

#define BTN_COIN           	0x0400  
#define BTN_TEST            0x0200  
#define BTN_SERVICE         0x4000  
#define BTN_CLEAR         	0x8000

PIUIO_MK5_STATE piuio_state;



unsigned short PIUIO_HandleInput_1(){
    unsigned short value = 0;
    if(piuio_state.p1_ul){value |= FP_LEFTUP;}
    if(piuio_state.p1_ur){value |= FP_RIGHTUP;}
    if(piuio_state.p1_center){value |= FP_CENTER;}
    if(piuio_state.p1_dl){value |= FP_LEFTDOWN;}
    if(piuio_state.p1_dr){value |= FP_RIGHTDOWN;}
    if(piuio_state.btn_test){value |= BTN_TEST;}
    if(piuio_state.btn_coin_1){value |= BTN_COIN;}
    if(piuio_state.btn_service){value |= BTN_SERVICE;}
    if(piuio_state.btn_clear){value |= BTN_CLEAR;}
    return ~value;
}
unsigned short PIUIO_HandleInput_2(){

    unsigned short value = 0;
    if(piuio_state.p2_ul){value |= FP_LEFTUP;}
    if(piuio_state.p2_ur){value |= FP_RIGHTUP;}
    if(piuio_state.p2_center){value |= FP_CENTER;}
    if(piuio_state.p2_dl){value |= FP_LEFTDOWN;}
    if(piuio_state.p2_dr){value |= FP_RIGHTDOWN;}
    if(piuio_state.btn_coin_2){value |= BTN_COIN;}
    return ~value;
}

void PIUIO_HandleOutput_1(unsigned short val){
    // TODO
}

void PIUIO_HandleOutput_2(unsigned short val){
    // TODO
}

void PIUIO_Init(){
    memset(&piuio_state,0,sizeof(PIUIO_MK5_STATE));
}