#include <stdio.h>
#include <stdint.h>

#include "pcmctrl.h"

static char * const reg_names [] =
{
    "Identification and Revision",
    "Interface Status",
    "Power and RESETDRV Control",
    "Interrupt and General Control",
    "Card Status Change",
    "Card Status Change Interrupt Configuration",
    "Address Window Enable",
    "I/O Control",
    "I/O Address 0 Start Low Byte",
    "I/O Address 0 Start High Byte",
    "I/O Address 0 Stop Low Byte",
    "I/O Address 0 Stop High Byte",
    "I/O Address 1 Start Low Byte",
    "I/O Address 1 Start High Byte",
    "I/O Address 1 Stop Low Byte",
    "I/O Address 1 Stop High Byte",
    "System Memory Address 0 Mapping Start Low Byte",
    "System Memory Address 0 Mapping Start High Byte",
    "System Memory Address 0 Mapping Stop Low Byte",
    "System Memory Address 0 Mapping Stop High Byte",
    "Card Memory Offset Address 0 Low Byte",
    "Card Memory Offset Address 0 High Byte",
    "Card Detect and General Control Register",
    NULL,
    "System Memory Address 1 Mapping Start Low Byte",
    "System Memory Address 1 Mapping Start High Byte",
    "System Memory Address 1 Mapping Stop Low Byte",
    "System Memory Address 1 Mapping Stop High Byte",
    "Card Memory Offset Address 1 Low Byte",
    "Card Memory Offset Address 1 High Byte",
    "Global Control Register",
    "Card Voltage Sense Register",
    "System Memory Address 2 Mapping Start Low Byte",
    "System Memory Address 2 Mapping Start High Byte",
    "System Memory Address 2 Mapping Stop Low Byte",
    "System Memory Address 2 Mapping Stop High Byte",
    "Card Memory Offset Address 2 Low Byte",
    "Card Memory Offset Address 2 High Byte",
    NULL,
    NULL,
    "System Memory Address 3 Mapping Start Low Byte",
    "System Memory Address 3 Mapping Start High Byte",
    "System Memory Address 3 Mapping Stop Low Byte",
    "System Memory Address 3 Mapping Stop High Byte",
    "Card Memory Offset Address 3 Low Byte",
    "Card Memory Offset Address 3 High Byte",
    NULL,
    "Card Voltage Select Register",
    "System Memory Address 4 Mapping Start Low Byte",
    "System Memory Address 4 Mapping Start High Byte",
    "System Memory Address 4 Mapping Stop Low Byte",
    "System Memory Address 4 Mapping Stop High Byte",
    "Card Memory Offset Address 4 Low Byte",
    "Card Memory Offset Address 4 High Byte",
};


int main()
{
    pcm_handle_t pcmo, * pcm;
    uint8_t socket;
    uint8_t reg_no;

    printf("DUMPPREG- Dump PCMCIA Controller Registers Test Utility\n");

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
            printf(">Socket %d\n", socket);
            for(reg_no = 0; reg_no < 0x36; reg_no++)
            {
                uint8_t reg_val;
                if(reg_names[reg_no] == NULL)
                {
                    continue;
                }

                reg_val = pcm_read(pcm, socket, reg_no);
                printf(">>%s (%X): %X\n", reg_names[reg_no], reg_no, reg_val);
            }
        }
    }

    return 0;
}
