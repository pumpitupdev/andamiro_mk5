#pragma once


typedef struct _PIUIO_MK5_STATE{
    unsigned char p1_ul;
    unsigned char p1_ur;
    unsigned char p1_center;
    unsigned char p1_dl;
    unsigned char p1_dr;
    unsigned char p2_ul;
    unsigned char p2_ur;
    unsigned char p2_center;
    unsigned char p2_dl;
    unsigned char p2_dr;    
    unsigned char btn_test;
    unsigned char btn_coin_1;
    unsigned char btn_coin_2;
    unsigned char btn_service;
    unsigned char btn_clear;
}PIUIO_MK5_STATE,*PPIUIO_MK5_STATE;

extern PIUIO_MK5_STATE piuio_state;


void PIUIO_HandleOutput_1(unsigned short val);
void PIUIO_HandleOutput_2(unsigned short val);
unsigned short PIUIO_HandleInput_1();
unsigned short PIUIO_HandleInput_2();
void PIUIO_Init();