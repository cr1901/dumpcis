#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <stdint.h>

typedef uint16_t pcm_handle_t;

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


int main()
{
    uint8_t irr;
    pcm_handle_t pcm;

    printf("DUMPCIS- Dump Card\n");
    irr = pcm_read(pcm, 0x00);
    printf("IIR: %X\n", irr);

    return 0;
}
