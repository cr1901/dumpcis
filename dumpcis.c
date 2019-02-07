#include <stdlib.h>
#include <stdio.h>
#include "pcmctrl.h"

uint8_t __far * PCM_WIN = (uint8_t __far *) 0xe0000000;

int main()
{
    pcm_handle_t pcmo, * pcm;
    int socket;

    printf("DUMPCIS- Dump PCMCIA Card Information Structure\n");

    for(pcm = pcm_find_init(&pcmo); pcm_find_done(pcm); pcm = pcm_find_next(pcm))
    {
        uint8_t irr = pcm_read(pcm, 0, 0x00);
        if(((irr & 0xF) == 3) || ((irr & 0xF) == 4))
        {
            printf("Found controller at %X.\n", pcm_ioaddr(pcm));
        }
        else
        {
            continue;
        }

        for(socket = 0; socket < 2; socket++)
        {
            uint8_t isr = pcm_read(pcm, socket, 0x01);
            uint8_t power = 0;

            if((isr & 0xC))
            {
                printf("Found a card in Socket %d.\n", socket);
            }
            else
            {
                continue;
            }

            power = pcm_read(pcm, socket, 0x02);
            printf("Power: %X\n", power);
        }
    }

    return 0;
}
