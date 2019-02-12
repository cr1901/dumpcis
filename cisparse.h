#ifndef CISPARSE_H
#define CISPARSE_H

#include <stdint.h>
#include <stdbool.h>


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
        struct
        {
            uint8_t space_no;
            uint32_t addr;
        } * addrs;
    } longlink_mfc;

    /* CISTPL_NO_LINK has no body. */
    /* CISTPL_NULL has no body. */

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
    /* No need to do a deep copy of each cis_tuple_t- alloc will ensure the
    memory used by each tuple's pointers won't go out of scope. */
    bool (* foreach)(cis_tuple_t curr, void * user);
    bool (* alloc)(void ** mem_ptr, size_t size, void * user);
    void * user;
} cis_parser_t;

#endif
