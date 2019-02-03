#include <stdint.h>
#include <conio.h>
#include "pcmctrl.h"

int pcm_find(pcm_handle_t * pcm)
{
    return 0;
}

uint8_t pcm_read_data(pcm_handle_t pcm)
{
    return inp(0x3E1);
}

void pcm_write_data(pcm_handle_t pcm, uint8_t data)
{
    outp(0x3E1, data);
}

void pcm_set_addr(pcm_handle_t pcm, uint8_t addr)
{
    outp(0x3E0, addr);
}

uint8_t pcm_read(pcm_handle_t pcm, uint8_t addr)
{
    pcm_set_addr(pcm, addr);
    return pcm_read_data(pcm);
}

void pcm_write(pcm_handle_t pcm, uint8_t addr, uint8_t data)
{
    pcm_set_addr(pcm, addr);
    pcm_write_data(pcm, data);
}
