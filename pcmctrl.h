#ifndef PCMCTRL_H
#define PCMCTRL_H

#include <stdbool.h>
#include <stdint.h>

#define PCM_ADDR_SIZE 0x04000000uL

typedef struct
{
    uint16_t io_base;
    uint8_t id;
    bool pnp;
    bool valid;
} pcm_handle_t;

typedef struct
{
    uint8_t __far * isa_win;
    uint32_t pcm_start;
    uint16_t num_blocks;
} pcm_window_t;

pcm_handle_t * pcm_find_init(pcm_handle_t * pcm);
pcm_handle_t * pcm_find_next(pcm_handle_t * pcm);
bool pcm_find_done(pcm_handle_t * pcm);

bool pcm_valid(pcm_handle_t * pcm);
uint16_t pcm_ioaddr(pcm_handle_t * pcm);

uint8_t pcm_read_data(pcm_handle_t * pcm);
void pcm_write_data(pcm_handle_t * pcm, uint8_t data);

void pcm_set_addr(pcm_handle_t * pcm, uint8_t socket, uint8_t addr);
uint8_t pcm_read(pcm_handle_t * pcm, uint8_t socket, uint8_t addr);
void pcm_write(pcm_handle_t * pcm, uint8_t socket, uint8_t addr, uint8_t data);

void pcm_get_window(pcm_handle_t * pcm, uint8_t socket, uint8_t win_num, \
    pcm_window_t * w_info);
void pcm_map_window(pcm_handle_t * pcm, uint8_t socket, uint8_t win_num, \
    pcm_window_t * w_info);

#endif
