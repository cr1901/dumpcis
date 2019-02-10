#include <stdlib.h>
#include <stdio.h>

#include <dos.h>

#include "pcmctrl.h"

uint8_t __far * PCM_WIN = (uint8_t __far *) MK_FP(0xe000, 0000);
#define PCM_WIN_LINEAR 0xe0000uL
#define PCM_ADDR_SIZE 0x04000000uL
#define PCM_OFFSET(_isa, _pcm) ((_pcm - _isa) % PCM_ADDR_SIZE)

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
            uint8_t prev_power = 0, prev_adrwin = 0, \
                prev_sysmap0_start_lo = 0, prev_sysmap0_start_hi = 0, \
                prev_sysmap0_stop_lo = 0, prev_sysmap0_stop_hi = 0, \
                prev_offset0_lo = 0, prev_offset0_hi = 0;

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

            /* CIS begins at addr 0 in PCMCIA attribute memory. */
            offset = PCM_OFFSET(PCM_WIN_LINEAR, 0);

            prev_sysmap0_start_lo = pcm_read(pcm, socket, 0x10);
            pcm_write(pcm, socket, 0x10, PCM_WIN_LINEAR >> 12);

            prev_sysmap0_start_hi = pcm_read(pcm, socket, 0x11);
            tmp = (prev_sysmap0_start_hi & 0xC0) | ((PCM_WIN_LINEAR >> 20) & 0x0FuL);
            pcm_write(pcm, socket, 0x11, tmp);

            prev_sysmap0_stop_lo = pcm_read(pcm, socket, 0x12);
            pcm_write(pcm, socket, 0x12, PCM_WIN_LINEAR >> 12);

            /* Allocate 8kB window, as stop address is 4kB inclusive. */
            prev_sysmap0_stop_hi = pcm_read(pcm, socket, 0x13);
            tmp = (prev_sysmap0_stop_hi & 0xC0) | ((PCM_WIN_LINEAR >> 20) & 0x0FuL);
            tmp += 0x1000;
            pcm_write(pcm, socket, 0x13, tmp);

            prev_offset0_lo = pcm_read(pcm, socket, 0x14);
            pcm_write(pcm, socket, 0x14, offset >> 12);

            prev_offset0_hi = pcm_read(pcm, socket, 0x15);
            tmp = (prev_offset0_hi & 0xC0) | ((offset >> 20) & 0x3FuL);
            pcm_write(pcm, socket, 0x15, tmp);

            /* Enable attribute memory and read CIS. */
            pcm_write(pcm, socket, 0x06, 0x01);

            for(count = 0; count < 254; count++)
            {
                uint8_t dat = PCM_WIN[count];
                printf("%c", dat);
            }

            pcm_write(pcm, socket, 0x06, 0);

            /* Restore modified registers. */
            pcm_write(pcm, socket, 0x15, prev_offset0_hi);
            pcm_write(pcm, socket, 0x14, prev_offset0_lo);
            pcm_write(pcm, socket, 0x13, prev_sysmap0_stop_hi);
            pcm_write(pcm, socket, 0x12, prev_sysmap0_stop_lo);
            pcm_write(pcm, socket, 0x11, prev_sysmap0_start_hi);
            pcm_write(pcm, socket, 0x10, prev_sysmap0_start_lo);

            pcm_write(pcm, socket, 0x06, prev_adrwin);
            pcm_write(pcm, socket, 0x02, prev_power);
        }
    }

    return 0;
}
