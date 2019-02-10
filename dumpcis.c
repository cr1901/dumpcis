#include <stdlib.h>
#include <stdio.h>

#include <dos.h>

#include "pcmctrl.h"

uint8_t __far * PCM_WIN = (uint8_t __far *) MK_FP(0xe000, 0000);

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
            /* Preserve as much previous state as we can, be as minimally
            invasive as possible. */
            uint8_t prev_power = 0, prev_adrwin = 0;
            pcm_window_t prev_win;

            pcm_window_t curr_window;

            uint8_t count = 0;
            uint32_t offset = 0;
            uint32_t tmp = 0;

            uint8_t isr = pcm_read(pcm, socket, 0x01);
            if((isr & 0xC))
            {
                printf("Found a card in Socket %d.\n", socket);
            }
            else
            {
                continue;
            }

            /* Power socket on. */
            prev_power = pcm_read(pcm, socket, 0x02);
            pcm_write(pcm, socket, 0x02, prev_power | (1 << 4));
            while((pcm_read(pcm, socket, 0x01) & 0x40) == 0);

            prev_adrwin = pcm_read(pcm, socket, 0x06);
            pcm_write(pcm, socket, 0x06, 0); /* Disable all windows so we can
                                                get our own window to read CIS. */

            pcm_get_window(pcm, socket, 0, &prev_win);

            /* CIS begins at addr 0 in PCMCIA attribute memory. */
            curr_window.isa_win = PCM_WIN;
            curr_window.pcm_start = 0;
            curr_window.num_blocks = 2;
            pcm_map_window(pcm, socket, 0, &curr_window);

            /* Enable attribute memory and read CIS. */
            pcm_write(pcm, socket, 0x06, 0x01);

            for(count = 0; count < 254; count++)
            {
                uint8_t dat = PCM_WIN[count];
                printf("%c", dat);
            }

            pcm_write(pcm, socket, 0x06, 0);

            /* Restore modified registers. */
            pcm_map_window(pcm, socket, 0, &prev_win);

            pcm_write(pcm, socket, 0x06, prev_adrwin);
            pcm_write(pcm, socket, 0x02, prev_power);
        }
    }

    return 0;
}
