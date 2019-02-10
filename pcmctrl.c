#include <stdint.h>
#include <stdbool.h>

#include <conio.h>
#include <dos.h>

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

#define MK_ISA(_h, _l) (((uint32_t) (_h & 0x0FuL) << 20) | (_l << 12))
#define MK_OFFSET(_h, _l) (((uint32_t) (_h & 0x3FuL) << 20) | (_l << 12))
// Normalized Far Pointer
#define LIN_TO_FP(_a) MK_FP((_a & 0x0FFFFFuL) >> 4, (_a & 0x0FuL))
#define WIN_OFFS(_w) (_w << 3)
void pcm_get_window(pcm_handle_t * pcm, uint8_t socket, uint8_t win_num, \
    pcm_window_t * w_info)
{
    uint8_t sysmap_start_lo, sysmap_start_hi;
    uint8_t sysmap_stop_lo, sysmap_stop_hi;
    uint8_t offset_hi, offset_lo;
    uint32_t full_addr_start, full_addr_stop, full_addr_offset;

    sysmap_start_lo = pcm_read(pcm, socket, 0x10 + WIN_OFFS(win_num));
    sysmap_start_hi = pcm_read(pcm, socket, 0x11 + WIN_OFFS(win_num));
    full_addr_start = MK_ISA(sysmap_start_hi, sysmap_start_lo);

    sysmap_stop_lo = pcm_read(pcm, socket, 0x12 + WIN_OFFS(win_num));
    sysmap_stop_hi = pcm_read(pcm, socket, 0x13 + WIN_OFFS(win_num));
    full_addr_stop = MK_ISA(sysmap_stop_hi, sysmap_stop_lo);

    w_info->num_blocks = ((full_addr_stop - full_addr_start) >> 12) + 1;

    offset_lo = pcm_read(pcm, socket, 0x14 + WIN_OFFS(win_num));
    offset_hi = pcm_read(pcm, socket, 0x15 + WIN_OFFS(win_num));
    full_addr_offset = MK_OFFSET(offset_hi, offset_lo);

    w_info->pcm_start = (full_addr_offset + full_addr_start) % PCM_ADDR_SIZE;
    w_info->isa_win = LIN_TO_FP(full_addr_start);
}


#define FP_TO_LIN(_a) (((uint32_t) FP_SEG(_a) << 4) + FP_OFF(_a))
#define PCM_OFFSET(_isa, _pcm) ((_pcm - _isa) % PCM_ADDR_SIZE)
void pcm_map_window(pcm_handle_t * pcm, uint8_t socket, uint8_t win_num, \
    pcm_window_t * w_info)
{
    uint32_t offset;
    uint32_t isa_lin;
    uint8_t tmp;

    isa_lin = FP_TO_LIN(w_info->isa_win);
    offset = PCM_OFFSET(isa_lin, w_info->pcm_start);

    // Only modify the bits that influence the window for each register.
    // System Start
    pcm_write(pcm, socket, 0x10 + WIN_OFFS(win_num), isa_lin >> 12);
    tmp = pcm_read(pcm, socket, 0x11 + WIN_OFFS(win_num));
    tmp = (tmp & 0xC0) | ((isa_lin >> 20) & 0x0FuL);
    pcm_write(pcm, socket, 0x11 + WIN_OFFS(win_num), tmp);

    // System Stop Address- stop address is 4kB inclusive.
    isa_lin += (0x1000 * (w_info->num_blocks - 1));
    pcm_write(pcm, socket, 0x12 + WIN_OFFS(win_num), isa_lin >> 12);
    tmp = pcm_read(pcm, socket, 0x13 + WIN_OFFS(win_num));
    tmp = (tmp & 0xC0) | ((isa_lin >> 20) & 0x0FuL);
    pcm_write(pcm, socket, 0x13 + WIN_OFFS(win_num), tmp);

    // Offset
    pcm_write(pcm, socket, 0x14 + WIN_OFFS(win_num), offset >> 12);
    tmp = pcm_read(pcm, socket, 0x15 + WIN_OFFS(win_num));
    tmp = (tmp & 0xC0) | ((offset >> 20) & 0x3FuL);
    pcm_write(pcm, socket, 0x15 + WIN_OFFS(win_num), tmp);
}
