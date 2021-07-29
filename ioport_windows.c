#include <Windows.h>

#include "sigdriver.h"
#include "am030614.h"
#include "at93c86.h"
#include "cat702.h"

#include "ioport.h"



LONG WINAPI ioport_handler(PEXCEPTION_POINTERS pExceptionInfo){
    // If this isn't what we want, skip it...
    if(pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_PRIV_INSTRUCTION) {
        if(pExceptionInfo->ExceptionRecord->ExceptionCode!= DBG_PRINTEXCEPTION_C){
        }
        return EXCEPTION_EXECUTE_HANDLER;
    }

    // -- Handle OUT DX,AL --
    unsigned int eip_val = *(unsigned int*)pExceptionInfo->ContextRecord->Eip;
    if((eip_val & 0xFF) == 0xEE){
        switch(pExceptionInfo->ContextRecord->Edx & 0xFF){
            case IOPORT_SIGDRIVER_OUT:
                SigDriver_HandleOutput(pExceptionInfo->ContextRecord->Eax & 0xFF);
                break;
            default:
                break;
        }
        pExceptionInfo->ContextRecord->Eip++;
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // -- Handle OUT DX,AX --
    if((eip_val & 0xFFFF) == 0xEF66){
        switch(pExceptionInfo->ContextRecord->Edx  & 0xFFFF){
            case IOPORT_PIUIO_P1_OUT:
                PIUIO_HandleOutput_1(pExceptionInfo->ContextRecord->Eax & 0xFFFF);
                break;
            case IOPORT_PIUIO_P2_OUT:
                PIUIO_HandleOutput_2(pExceptionInfo->ContextRecord->Eax & 0xFFFF);
                break;   
            case IOPORT_AT93C86_CAT702_OUT:
                if(pExceptionInfo->ContextRecord->Eax & 0xFF00){
                    CAT702_HandleOutput(pExceptionInfo->ContextRecord->Eax & 0xFFFF);
                }else{
                    AT93C86_HandleOutput(pExceptionInfo->ContextRecord->Eax & 0xFFFF);
                }                
                break;
            default:
                break;
       }
        pExceptionInfo->ContextRecord->Eip+=2;
        return EXCEPTION_CONTINUE_EXECUTION;
   }

    // -- Handle IN AX,DX --
    if((eip_val & 0xFFFF) == 0xED66){
        switch(pExceptionInfo->ContextRecord->Edx & 0xFFFF){
            case IOPORT_PIUIO_P1_IN:
                pExceptionInfo->ContextRecord->Eax = PIUIO_HandleInput_1();
                break;
            case IOPORT_PIUIO_P2_IN:
                pExceptionInfo->ContextRecord->Eax = PIUIO_HandleInput_2();
                break;              
            case IOPORT_CAT702_IN:
                pExceptionInfo->ContextRecord->Eax = CAT702_HandleInput();
                break;
            case IOPORT_AT93C86_IN:
                pExceptionInfo->ContextRecord->Eax = AT93C86_HandleInput();
                break;  
            default:
                break;
        }
        pExceptionInfo->ContextRecord->Eip+=2;
        return EXCEPTION_CONTINUE_EXECUTION;
   }

    return EXCEPTION_EXECUTE_HANDLER;
}


void IOPort_Init(){
    AddVectoredExceptionHandler(1,ioport_handler);
}