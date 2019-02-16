#ifndef CISPARSE_H
#define CISPARSE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* CIS tuple data types which can be variable length. */
typedef struct
{
    uint8_t space_no;
    uint32_t addr;
} cis_mfc_addr_t;

typedef enum
{
    ALLOC_CHAR_STR = 0, /* Alloc space for a C string. */
    ALLOC_CHAR_PTR, /* Alloc space for an array of char ptrs (ptrs to strings). */
    ALLOC_MFC_ADDR,
    ALLOC_DEV_INFO
} cis_alloc_req_t;

/* Below fields are technically variable, but are not implemented as such.
The spec reserves variable space for future extensions, but doesn't give
meaning to said bits.
TODO: Parser behavior is to ignore, warn, or error by option, not just ignore. */
typedef struct
{
    uint8_t exponent;
    uint8_t mantissa;
} cis_ext_speed_t;

typedef uint8_t cis_ext_type_t;

typedef struct
{
    uint8_t vcc_used;
    uint8_t mwait;
} cis_other_cond_t;

typedef struct
{
    uint8_t type;
    uint8_t wps;

    /* FIXME: The parser/application knows ahead of time whether a 16-bit
    PC card or CardBus is present? */
    union
    {
        uint8_t space_no;
        uint8_t speed;
    } speed_or_space;

    cis_ext_speed_t ext_speed;
    cis_ext_type_t ext_type;
    uint8_t size; /* Encodes in-band information about whether to ignore
                     other entries (size == 0xFF). Use macro to convert
                     to actual size. */
} cis_device_info_t;


/* Union fields are stored in order they are presented in the spec
(Page 18 of the Metaformat Spec). */
typedef union
{
    /* CISTPL_CHECKSUM */
    struct
    {
        /* TODO: Don't actually ignore this. */
        uint16_t addr;
        uint16_t len;
        uint8_t cs;
    } checksum;

    /* CISTPL_END has no body. */
    /* CISTPL_INDIRECT has no body. */
    /* CISTPL_LINKTARGET */
    struct
    {
        uint8_t tag[3];
    } linktarget;

    /* CISTPL_LONGLINK_A, CISTPL_LONGLINK_C */
    struct
    {
        uint32_t addr;
    } longlink_ac;

    /* CISTPL_LONGLINK_CB */
    struct
    {
        struct
        {
            uint8_t space_no;
            union
            {
                struct
                {
                    uint8_t offset;
                } config;

                struct
                {
                    uint32_t offset;
                } memory;

                struct
                {
                    uint32_t offset;
                    uint8_t image_no;
                } expansion;
            } space;
        } addr;
    } longlink_cb;

    /* CISTPL_LONGLINK_MFC */
    struct
    {
        uint8_t num_sets;
        cis_mfc_addr_t (* addrs)[];
    } longlink_mfc;

    /* CISTPL_NO_LINK has no body. */
    /* CISTPL_NULL has no body. */
    /* CISTPL_ALTSTR */
    struct
    {
        uint8_t num_strs;
        char * esc;
        char * (* strs)[];
    } altstr;

    /* CISTPL_DEVICE, CISTPL_DEVICE_A, CISTPL_DEVICE_OC, CISTPL_DEVICE_OA */
    struct
    {
        uint8_t num_devs;
        cis_other_cond_t other;
        cis_device_info_t (* devs)[];
    } device;

} cis_tuple_body_t;

typedef enum
{
    /* Layer 1 */
    CISTPL_NULL = 0,
    CISTPL_DEVICE,
    CISTPL_LONGLINK_CB,
    CISTPL_INDIRECT,
    CISTPL_CONFIG_CB,
    CISTPL_CFTABLE_ENTRY_CB,
    CISTPL_LONGLINK_MFC,
    CISTPL_BAR,
    CISTPL_PWR_MGMNT,
    CISTPL_EXTDEVICE,
    CISTPL_CHECKSUM = 0x10,
    CISTPL_LONGLINK_A,
    CISTPL_LONGLINK_C,
    CISTPL_LINKTARGET,
    CISTPL_NO_LINK,
    CISTPL_VERS_1,
    CISTPL_ALTSTR,
    CISTPL_DEVICE_A,
    CISTPL_JEDEC_C,
    CISTPL_JEDEC_A,
    CISTPL_CONFIG,
    CISTPL_CFTABLE_ENTRY,
    CISTPL_DEVICE_OC,
    CISTPL_DEVICE_OA,
    CISTPL_DEVICEGEO,
    CISTPL_DEVICEGEO_A,
    CISTPL_MANFID,
    CISTPL_FUNCID,
    CISTPL_FUNCE,
    /* Layer 2 */
    CISTPL_SWIL,
    CISTPL_VERS_2 = 0x40,
    CISTPL_FORMAT,
    CISTPL_GEOMETRY,
    CISTPL_BYTEORDER,
    CISTPL_DATE,
    CISTPL_BATTERY,
    /* Layer 3 */
    CISTPL_ORG,
    /* Layer 2 */
    CISTPL_FORMAT_A,
    /* Layer 4 */
    CISTPL_SPCL = 0x90,
    /* Layer 1 */
    CISTPL_END = 0xFF
} cis_tuple_type_t;

typedef struct
{
    cis_tuple_type_t type;
    cis_tuple_body_t body;
} cis_tuple_t;

typedef struct
{
    /* Public interface. */
    /* No need to do a deep copy of each cis_tuple_t- alloc will ensure the
    memory used by each tuple's pointers won't go out of scope. */
    /* Return true for success, false for "unable to complete task". */
    bool (* foreach)(cis_tuple_t curr, void * user);
    bool (* alloc)(void ** mem_ptr, cis_alloc_req_t type, size_t size, void * user);
    bool debug; /* Print parser debugging info if true. */
    void * user;

    /* Private interface follows. */
} cis_parser_t;


/* typedef enum
{

} parse_fail_reason_t; */

typedef struct
{
    enum
    {
        INTERNAL_OK = 0,
        UNKNOWN_TUPLE,
        BAD_VAR_ALLOC
    } err;

    union
    {
        uint8_t tuple_no;
        cis_alloc_req_t alloc_type;
    } meta;
} cis_internal_error_t;

typedef struct
{
    enum
    {
        PARSER_OK = 0,
        PARSE_BODY_FAILED,
        BAD_TUPLE_ALLOC
    } err;

    cis_internal_error_t reason;
} cis_parser_error_t;

cis_parser_error_t cis_parse(cis_parser_t * state, uint8_t __far * start_addr);

#endif
