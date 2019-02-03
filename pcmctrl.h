#ifndef PCMCTRL_H
#define PCMCTRL_H

#include <stdint.h>

typedef uint16_t pcm_handle_t;

int pcm_find(pcm_handle_t * pcm);
uint8_t pcm_read_data(pcm_handle_t pcm);
void pcm_write_data(pcm_handle_t pcm, uint8_t data);

void pcm_set_addr(pcm_handle_t pcm, uint8_t socket, uint8_t addr);
uint8_t pcm_read(pcm_handle_t pcm, uint8_t socket, uint8_t addr);
void pcm_write(pcm_handle_t pcm, uint8_t socket, uint8_t addr, uint8_t data);

#endif
