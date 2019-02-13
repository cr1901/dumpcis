#include <stdio.h>

#include "cisparse.h"
#include "pcmctrl.h"

#define CIS_TUPLE_TYPE(_b)

static bool inline is_end_tuple(cis_tuple_t tuple);
static int inline parse_body(cis_parser_t * state, cis_tuple_t * tuple, char body[], uint8_t len);
static uint16_t inline read_entry(char buf[], uint8_t __far * start, bool use_16);

int cis_parse(cis_parser_t * state, uint8_t __far * start_addr)
{
    char buf[257] = { '\0' };
    uint8_t tup_count;
    cis_tuple_t curr_tuple = { 0, 0 };

    for(tup_count = 0; (!is_end_tuple(curr_tuple) && tup_count < 255); tup_count++)
    {
        uint8_t byte_count;
        uint16_t len;

        len = read_entry(buf, start_addr, true);

        curr_tuple.type = buf[0];
        parse_body(state, &curr_tuple, &buf[2], buf[1]);

        for(byte_count = 0; byte_count < len; byte_count++)
        {
            printf("%X ", buf[byte_count]);
        }

        start_addr += 2*len; /* TODO: Support 8-bit meaningfully. */
        printf("next: %p\n", start_addr);
    }

    return 0;
}

static bool inline is_end_tuple(cis_tuple_t tuple)
{
    cis_tuple_type_t type = tuple.type;
    return (type == CISTPL_NO_LINK || type == CISTPL_END);
}

static int inline parse_body(cis_parser_t * state, cis_tuple_t * tuple, char body[], uint8_t len)
{
    return 0;
}

/* Returns number of chars read, to get body length, read buf[1]  or subtract 2
   from return value. */
static uint16_t inline read_entry(char buf[], uint8_t __far * start, bool use_16)
{
    uint16_t offset_pcm = 0;
    uint8_t incr = use_16 ? 2 : 1;
    uint16_t len = 0;
    uint16_t count;

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
