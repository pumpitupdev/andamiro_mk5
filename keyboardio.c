#include "am030614.h"

#if defined _WIN32
#include <Windows.h>
unsigned short update_input_state(){
    while(1){
        if ((GetAsyncKeyState(VK_F12) & 0x8000)) {exit(0); }
        // Foot Panels
        piuio_state.p1_ul =     (GetAsyncKeyState('Q') & 0x8000) ? 1 : 0;
        piuio_state.p1_ur =     (GetAsyncKeyState('E') & 0x8000) ?  1 : 0;
        piuio_state.p1_center = (GetAsyncKeyState('S') & 0x8000) ? 1 : 0;
        piuio_state.p1_dl =     (GetAsyncKeyState('Z') & 0x8000) ? 1 : 0;
        piuio_state.p1_dr =     (GetAsyncKeyState('C') & 0x8000) ? 1 : 0;

        piuio_state.p2_ul =     (GetAsyncKeyState('R') & 0x8000) ? 1 : 0;
        piuio_state.p2_ur =     (GetAsyncKeyState('Y') & 0x8000) ?  1 : 0;
        piuio_state.p2_center = (GetAsyncKeyState('G') & 0x8000) ? 1 : 0;
        piuio_state.p2_dl =     (GetAsyncKeyState('V') & 0x8000) ? 1 : 0;
        piuio_state.p2_dr =     (GetAsyncKeyState('N') & 0x8000) ? 1 : 0;

        // Switches
        piuio_state.btn_test = (GetAsyncKeyState('1') & 0x8000) ? 1 : 0;
        piuio_state.btn_service = (GetAsyncKeyState('2') & 0x8000) ? 1 : 0;
        piuio_state.btn_clear = (GetAsyncKeyState('3') & 0x8000) ? 1 : 0;
        piuio_state.btn_coin_1 = (GetAsyncKeyState('5') & 0x8000) ? 1 : 0;
        piuio_state.btn_coin_2 = (GetAsyncKeyState('6') & 0x8000) ? 1 : 0;
    }
}
#else
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/io.h>
#include <linux/input.h>
#include <pthread.h>
static int fd = -1;
static pthread_t hthread;
static void *input_thread(void *arg)
{
    struct input_event ev;
    int i;
	
    while (1) {
        read(fd, &ev, sizeof(ev));
        if (ev.type == EV_KEY) {
            switch(ev.code){
                // Foot Panels - P1
                case KEY_Q:
                    piuio_state.p1_ul = (ev.value) ? 1 : 0;
                    break;
                case KEY_E:
                    piuio_state.p1_ur = (ev.value) ? 1 : 0;
                    break;
                case KEY_S:
                    piuio_state.p1_center = (ev.value) ? 1 : 0;
                    break;
                case KEY_Z:
                    piuio_state.p1_dl = (ev.value) ? 1 : 0;
                    break;
                case KEY_C:
                    piuio_state.p1_dr = (ev.value) ? 1 : 0;
                    break;  
                // Foot Panels - P2
                case KEY_R:
                    piuio_state.p2_ul = (ev.value) ? 1 : 0;
                    break;
                case KEY_Y:
                    piuio_state.p2_ur = (ev.value) ? 1 : 0;
                    break;
                case KEY_G:
                    piuio_state.p2_center = (ev.value) ? 1 : 0;
                    break;
                case KEY_V:
                    piuio_state.p2_dl = (ev.value) ? 1 : 0;
                    break;
                case KEY_N:
                    piuio_state.p2_dr = (ev.value) ? 1 : 0;
                    break; 
                // Switches
                case KEY_1:
                    piuio_state.btn_test = (ev.value) ? 1 : 0;
                    break; 
                case KEY_2:
                    piuio_state.btn_service = (ev.value) ? 1 : 0;
                    break; 
                case KEY_3:
                    piuio_state.btn_clear = (ev.value) ? 1 : 0;
                    break; 
                case KEY_5:
                    piuio_state.btn_coin_1 = (ev.value) ? 1 : 0;
                    break; 
                case KEY_6:
                    piuio_state.btn_coin_2 = (ev.value) ? 1 : 0;
                    break;                                                                                                                                                                                                     
            }           
        }
    }
}
#endif

unsigned short update_output_state(){
    // TODO - Hook this up to OpenRGB
    while(1){}
}

void KeyboardIO_Init(){
#ifdef _WIN32
    CreateThread(0,0,(LPTHREAD_START_ROUTINE)update_input_state,0,0,0);
    CreateThread(0,0,(LPTHREAD_START_ROUTINE)update_output_state,0,0,0);    
#else
fd = open("/run/kbdhook", 0);
if (fd == -1) {
    perror("Opening input");
    exit(1);
}
pthread_create(&hthread, NULL, input_thread, NULL);
#endif
}

