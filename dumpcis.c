#include <stdlib.h>
#include <stdio.h>
#include "pcmctrl.h"

int main()
{
    uint8_t irr;
    pcm_handle_t pcm;

    printf("DUMPCIS- Dump Card\n");
    irr = pcm_read(pcm, 0x00);
    printf("IIR: %X\n", irr);

    return 0;
}
