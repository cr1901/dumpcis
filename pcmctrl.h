#ifndef PCMCTRL_H
#define PCMCTRL_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t io_base;
    uint8_t id;
    bool pnp;
    bool valid;
} pcm_handle_t;

pcm_handle_t * pcm_find_init(pcm_handle_t * pcm);
pcm_handle_t * pcm_find_next(pcm_handle_t * pcm);
bool pcm_find_done(pcm_handle_t * pcm);

uint8_t pcm_read_data(pcm_handle_t * pcm);
void pcm_write_data(pcm_handle_t * pcm, uint8_t data);

void pcm_set_addr(pcm_handle_t * pcm, uint8_t socket, uint8_t addr);
uint8_t pcm_read(pcm_handle_t * pcm, uint8_t socket, uint8_t addr);
void pcm_write(pcm_handle_t * pcm, uint8_t socket, uint8_t addr, uint8_t data);

#endif
