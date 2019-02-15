#include <stdlib.h>
#include <stdio.h>

#include <dos.h>

#include "cisparse.h"
#include "pcmctrl.h"

uint8_t __far * PCM_WIN = (uint8_t __far *) MK_FP(0xe000, 0000);

/* No malloc- reserve a reasonable amount of space for all possible
   allocations. We can't use a char * buf because we can't get alignment of
   each struct in this compiler, among other things. */
/* Used by foreach_tuple. */
cis_tuple_t tuple[40];

/* Used by alloc_tuple. */
char string[2048];
char * str_ptr[128];
cis_mfc_addr_t mfc_addr[32];
cis_device_info_t device_info[32];


/* All point to next-free. */
typedef struct
{
    uint8_t tuple;
    uint8_t string;
    uint16_t str_ptr;
    uint8_t mfc_addr;
    uint8_t device_info;
} dumpcis_user_t;

typedef struct
{
    bool ok;
    uint8_t num_parsed;
    uint8_t last_tup;
} print_error_t;

typedef struct
{
    enum
    {
        NO_ERROR,
        PARSE_ERROR,
        PRINT_ERROR
    } err;

    union
    {
        cis_parser_error_t parse;
        print_error_t print;
    } reason;
} dumpcis_error_t;


void cis_perror(dumpcis_error_t err);
bool foreach_tuple(cis_tuple_t curr, void * user);
bool alloc_tuple(void ** mem_ptr, size_t size, void * user);

int main()
{
    pcm_handle_t pcmo, * pcm;
    int socket;
    dumpcis_error_t app_rc = { NO_ERROR, 0 };

    printf("DUMPCIS- Dump PCMCIA Card Information Structure\n");

    for(pcm = pcm_find_init(&pcmo); pcm_find_done(pcm); pcm = pcm_find_next(pcm))
    {
        uint8_t irr = pcm_read(pcm, 0, 0x00);
        if(((irr & 0xF) == 3) || ((irr & 0xF) == 4))
        {
            printf("Found controller at %X.\n", pcm_ioaddr(pcm));
        }
        else
        {
            continue;
        }

        for(socket = 0; socket < 2; socket++)
        {
            /* Preserve as much previous state as we can, be as minimally
            invasive as possible. */
            uint8_t prev_power = 0, prev_adrwin = 0;
            pcm_window_t prev_win, curr_window;

            cis_parser_t parser;
            cis_parser_error_t parser_rc;
            dumpcis_user_t user = { 0, 0, 0, 0, 0 };
            uint8_t count = 0;
            uint8_t isr;

            parser.foreach = foreach_tuple;
            parser.alloc = alloc_tuple;
            parser.debug = true;
            parser.user = &user;

            isr = pcm_read(pcm, socket, 0x01);
            if((isr & 0xC))
            {
                printf("Found a card in Socket %d.\n", socket);
            }
            else
            {
                continue;
            }

            /* Power socket on. */
            prev_power = pcm_read(pcm, socket, 0x02);
            pcm_write(pcm, socket, 0x02, prev_power | (1 << 4));
            while((pcm_read(pcm, socket, 0x01) & 0x40) == 0);

            prev_adrwin = pcm_read(pcm, socket, 0x06);
            pcm_write(pcm, socket, 0x06, 0); /* Disable all windows so we can
                                                get our own window to read CIS. */

            pcm_get_window(pcm, socket, 0, &prev_win);

            /* CIS begins at addr 0 in PCMCIA attribute memory. */
            curr_window.isa_win = PCM_WIN;
            curr_window.pcm_start = 0;
            curr_window.num_blocks = 2;
            pcm_map_window(pcm, socket, 0, &curr_window);

            /* Enable attribute memory and read CIS. */
            pcm_write(pcm, socket, 0x06, 0x01);

            /* For loop to handle longlink goes here. */
            parser_rc = cis_parse(&parser, PCM_WIN);
            if(parser_rc.err != PARSER_OK)
            {
                app_rc.err = PARSE_ERROR;
                app_rc.reason.parse = parser_rc;
            }

            pcm_write(pcm, socket, 0x06, 0);

            /* Restore modified registers. */
            pcm_map_window(pcm, socket, 0, &prev_win);

            pcm_write(pcm, socket, 0x06, prev_adrwin);
            pcm_write(pcm, socket, 0x02, prev_power);

            if(app_rc.err != NO_ERROR)
            {
                goto check_err;
            }
        }
    }

check_err:
    cis_perror(app_rc);
    return app_rc.err;
}

void cis_perror(dumpcis_error_t err)
{
    if(err.err == NO_ERROR)
    {
        return;
    }
    else if(err.err == PARSE_ERROR)
    {
        cis_parser_error_t p_err = err.reason.parse;
        if(p_err.err == PARSE_BODY_FAILED)
        {
            uint16_t err_subtype = p_err.reason.err;
            if(err_subtype == UNKNOWN_TUPLE)
            {
                uint8_t meta = p_err.reason.meta.tuple_no;
                printf("Error: Encountered unknown tuple type %d.\n",
                    meta);
            }
            else if(err_subtype == BAD_VAR_ALLOC)
            {
                cis_alloc_req_t meta = p_err.reason.meta.alloc_type;
                printf("Error: Parser could not allocate room for variable args,"
                    " allocation type %d.\n", meta);
            }
            else
            {
                printf("Internal Error: Unknown error type %d returned by"
                    " parse_body()!\n", err_subtype);
            }
        }
        else if(p_err.err == BAD_TUPLE_ALLOC)
        {
            /* TODO: Last allocated tuple as union data? */
            printf("Error: DUMPCIS could not allocate tuple list.\n");
        }
        else
        {
            printf("Internal Error: Unknown parser error type %d!\n", \
                err.reason.parse.err);
        }
    }
    else if(err.err == PRINT_ERROR)
    {
        printf("Error: Placeholder Print Error.\n");
    }
    else
    {
        printf("Internal Error: Unknown application error type %d!\n",
            err.err);
    }
}

/* Callbacks */
bool foreach_tuple(cis_tuple_t curr, void * user)
{
    dumpcis_user_t * ptrs = user;
    if((ptrs->tuple) < (sizeof(tuple)/sizeof(cis_tuple_t)))
    {
        tuple[ptrs->tuple++] = curr;
        return true;
    }

    return false;
}

bool alloc_tuple(void ** mem_ptr, size_t size, void * user)
{
    return false;
}
