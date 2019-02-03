#include <stdlib.h>
#include <stdio.h>
#include "pcmctrl.h"

int main()
{

    pcm_handle_t pcmo, * pcm;

    printf("DUMPCIS- Dump Card\n");

    for(pcm = pcm_find_init(&pcmo); pcm_find_done(pcm); pcm = pcm_find_next(pcm))
    {
        uint8_t irr = pcm_read(pcm, 0, 0x00);
        printf("IIR: %X\n", irr);
    }





    return 0;
}
