#include <stdio.h>

#include "cisparse.h"
#include "pcmctrl.h"

static uint16_t inline read_entry(char buf[], uint8_t __far * start, bool use_16);

int cis_parse(cis_parser_t * state, uint8_t __far * start_addr)
{
    char buf[257];
    uint8_t count, count2;
    uint16_t len;

    for(count2 = 0; !(buf[0] == 0x14 || buf[0] == 0xFF); count2++)
    {
        len = read_entry(buf, start_addr, true);

        for(count = 0; count < len; count++)
        {
            printf("%X ", buf[count]);
        }

        start_addr += 2*len; /* TODO: Support 8-bit meaningfully. */
        printf("next: %p\n", start_addr);
    }

    return 0;
}


/* Returns number of chars read. */
static uint16_t inline read_entry(char buf[], uint8_t __far * start, bool use_16)
{
    uint16_t offset_pcm = 0;
    uint8_t incr = use_16 ? 2 : 1;
    uint8_t len = 0;
    uint8_t count;

    buf[0] = *(start + offset_pcm);
    offset_pcm += incr;

    len = *(start + offset_pcm);
    buf[1] = len;
    offset_pcm += incr;

    for(count = 2; count < (len + 2); count++, offset_pcm += incr)
    {
        buf[count] = *(start + offset_pcm);
    }

    return len + 2;
}
