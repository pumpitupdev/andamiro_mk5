#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <ucontext.h>
#include <sys/io.h>

#include "sigdriver.h"
#include "am030614.h"
#include "at93c86.h"
#include "cat702.h"
#include "ioport.h"

void ioport_handler(int mysignal, siginfo_t *si, void* arg){    
  ucontext_t *context = (ucontext_t *)arg;    
  
  unsigned int eip_val = context->uc_mcontext.gregs[REG_EIP];
  // -- Handle OUT DX,AL --
  if((eip_val & 0xFF) == 0xEE){
      switch(context->uc_mcontext.gregs[REG_EDX] & 0xFF){
          case IOPORT_SIGDRIVER_OUT:
              SigDriver_HandleOutput(context->uc_mcontext.gregs[REG_EAX] & 0xFF);
              break;
          default:
              break;
      }
      context->uc_mcontext.gregs[REG_EIP]++;
  }

  // -- Handle OUT DX,AX --
  if((eip_val & 0xFFFF) == 0xEF66){
      switch(context->uc_mcontext.gregs[REG_EDX] & 0xFFFF){
          case IOPORT_PIUIO_P1_OUT:
              PIUIO_HandleOutput_1(context->uc_mcontext.gregs[REG_EAX] & 0xFFFF);
              break;
          case IOPORT_PIUIO_P2_OUT:
              PIUIO_HandleOutput_2(context->uc_mcontext.gregs[REG_EAX] & 0xFFFF);
              break;   
          case IOPORT_AT93C86_CAT702_OUT:
              if(context->uc_mcontext.gregs[REG_EAX] & 0xFF00){
                  CAT702_HandleOutput(context->uc_mcontext.gregs[REG_EAX] & 0xFFFF);
              }else{
                  AT93C86_HandleOutput(context->uc_mcontext.gregs[REG_EAX] & 0xFFFF);
              }                
              break;
          default:
              break;
      }
      context->uc_mcontext.gregs[REG_EIP]+=2;
  }

  // -- Handle IN AX,DX --
  if((eip_val & 0xFFFF) == 0xED66){
      switch(context->uc_mcontext.gregs[REG_EDX] & 0xFFFF){
          case IOPORT_PIUIO_P1_IN:
              context->uc_mcontext.gregs[REG_EAX] = PIUIO_HandleInput_1();
              break;
          case IOPORT_PIUIO_P2_IN:
              context->uc_mcontext.gregs[REG_EAX] = PIUIO_HandleInput_2();
              break;              
          case IOPORT_CAT702_IN:
              context->uc_mcontext.gregs[REG_EAX] = CAT702_HandleInput();
              break;
          case IOPORT_AT93C86_IN:
              context->uc_mcontext.gregs[REG_EAX] = AT93C86_HandleInput();
              break;  
          default:
              break;
      }
      context->uc_mcontext.gregs[REG_EIP]+=2;
  }

}
void IOPort_Init(){
  // We have to make sure the ioports aren't permitted.
  ioperm(IOPORT_PIUIO_P1_OUT, 16, 0);
  ioperm(IOPORT_SIGDRIVER_OUT,1, 0);
  struct sigaction action;
  action.sa_sigaction = &ioport_handler;
  action.sa_flags = SA_SIGINFO;
  sigaction(SIGILL,&action,NULL);  
}