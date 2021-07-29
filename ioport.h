#pragma once

#define IOPORT_SIGDRIVER_OUT      0x80

#define IOPORT_PIUIO_P1_OUT       0x2A0
#define IOPORT_PIUIO_P2_OUT       0x2A2
#define IOPORT_PIUIO_P1_IN        0x2A4
#define IOPORT_PIUIO_P2_IN        0x2A6

#define IOPORT_CAT702_IN          0x2AA
#define IOPORT_AT93C86_IN         0x2AC
#define IOPORT_AT93C86_CAT702_OUT 0x2AE

void IOPort_Init();