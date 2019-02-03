#include <stdint.h>
#include <stdbool.h>
#include <conio.h>
#include "pcmctrl.h"

#define SOCK_ADDR(_s, _a) ((_s == 0) ? 0x00 : 0x40) + _a

uint16_t io_bases[3] = {0x3E0, 0x3E2, 0x279};

pcm_handle_t * pcm_find_init(pcm_handle_t * pcm)
{
    pcm->io_base = io_bases[0];
    pcm->id = 0;
    pcm->pnp = false;
    pcm->valid = true;

    return pcm;
}

pcm_handle_t * pcm_find_next(pcm_handle_t * pcm)
{
    pcm->id++;

    if(pcm->id < 3)
    {
        pcm->io_base = io_bases[pcm->id];
        pcm->valid = true;
    }
    else
    {
        pcm->io_base = 0;
        pcm->valid = false;
    }

    pcm->pnp = (pcm->io_base == 0x279);

    return pcm;
}

bool pcm_find_done(pcm_handle_t * pcm)
{
    return !(pcm->id >= 3);
}

bool pcm_valid(pcm_handle_t * pcm)
{
    return pcm->valid;
}

uint16_t pcm_ioaddr(pcm_handle_t * pcm)
{
    return pcm->io_base;
}

uint8_t pcm_read_data(pcm_handle_t * pcm)
{
    return inp(pcm->io_base + 1);
}

void pcm_write_data(pcm_handle_t * pcm, uint8_t data)
{
    outp(pcm->io_base + 1, data);
}

void pcm_set_addr(pcm_handle_t * pcm, uint8_t socket, uint8_t addr)
{
    outp(pcm->io_base, SOCK_ADDR(socket, addr));
}

uint8_t pcm_read(pcm_handle_t * pcm, uint8_t socket, uint8_t addr)
{
    pcm_set_addr(pcm, socket, addr);
    return pcm_read_data(pcm);
}

void pcm_write(pcm_handle_t * pcm, uint8_t socket, uint8_t addr, uint8_t data)
{
    pcm_set_addr(pcm, socket, addr);
    pcm_write_data(pcm, data);
}
