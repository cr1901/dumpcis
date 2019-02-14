#include <stdio.h>

#include "cisparse.h"
#include "pcmctrl.h"

#define CIS_TUPLE_TYPE(_b)

static bool inline is_end_tuple(cis_tuple_t tuple);
static cis_internal_error_t inline parse_body(cis_parser_t * state, cis_tuple_t * tuple, char body[], uint8_t len);
static uint16_t inline read_entry(char buf[], uint8_t __far * start, bool use_16);

cis_parser_error_t cis_parse(cis_parser_t * state, uint8_t __far * start_addr)
{
    char buf[257] = { '\0' };
    uint8_t tup_count;
    cis_tuple_t curr_tuple = { CISTPL_NULL, 0 };
    cis_parser_error_t rc = { PARSER_OK, { 0, 0 } };

    for(tup_count = 0; (!is_end_tuple(curr_tuple) && tup_count < 255); tup_count++)
    {
        uint8_t byte_count;
        uint16_t len;
        cis_internal_error_t parse_rc;

        len = read_entry(buf, start_addr, true);

        curr_tuple.type = buf[0];
        parse_rc = parse_body(state, &curr_tuple, &buf[2], buf[1]);

        if(parse_rc.err != INTERNAL_OK)
        {
            rc.err = PARSE_BODY_FAILED;
            rc.reason = parse_rc;
            return rc;
        }
        else
        {
            bool foreach_rc;

            foreach_rc = state->foreach(curr_tuple, state->user);
            if(!foreach_rc)
            {
                rc.err = BAD_TUPLE_ALLOC;
                return rc;
            }
        }

        start_addr += 2*len; /* TODO: Support 8-bit meaningfully. */

        if(state->debug)
        {
            for(byte_count = 0; byte_count < len; byte_count++)
            {
                printf("%X ", buf[byte_count]);
            }
            printf("next: %p\n", start_addr);
        }
    }

    return rc;
}

static bool inline is_end_tuple(cis_tuple_t tuple)
{
    cis_tuple_type_t type = tuple.type;
    return (type == CISTPL_NO_LINK || type == CISTPL_END);
}

/* The important logic is here- we have a tuple, figure out how the body
   should be interpreted. */
static cis_internal_error_t inline parse_body(cis_parser_t * state, cis_tuple_t * tuple, char body[], uint8_t len)
{
    cis_internal_error_t rc;
    rc.err = INTERNAL_OK;

    return rc;
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
