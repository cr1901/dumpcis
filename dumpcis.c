#include <stdlib.h>
#include <stdio.h>
#include "pcmctrl.h"

int main()
{
    pcm_handle_t pcmo, * pcm;
    int socket;

    printf("DUMPCIS- Dump PCMCIA Card Information Structure\n");

    for(pcm = pcm_find_init(&pcmo); pcm_find_done(pcm); pcm = pcm_find_next(pcm))
    {
        for(socket = 0; socket < 2; socket++)
        {
            uint8_t irr = pcm_read(pcm, 0, 0x00);
            printf("IO base: %X, Socket: %d, IIR: %X\n", pcm_ioaddr(pcm), socket, irr);
        }
    }

    return 0;
}
