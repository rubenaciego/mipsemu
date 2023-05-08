#ifndef MIPS_CPU_H
#define MIPS_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define INS_SIZE 4
#define EXCP_ADDR 0x80000180

#define get_opcode(x) ((x >> 26) & 0b111111)


enum format
{
    RFORMAT, IFORMAT, JFORMAT
};

enum print_format
{
    ADDRESS, RDRTSHAMT, RDRTRS, RS, RDRS,
    NOTHING, RD, RSRT, RDRSRT, RSRTIMM, RSIMM,
    RTRSIMM, RTIMM, RTIMMRS, RDRT
};

enum exception
{
    INT = 0, ADDRL = 4, ADDRS = 5,
    IBUS = 6, DBUS = 7, SYSCALL = 8,
    BKPT = 9, RI = 10, OVF = 12, TRAP = 13,
    FPE = 15
};

struct mipscpu;
struct instruction;

struct operation
{
    char name[7];
    enum print_format pformat;
    void (*func)(struct mipscpu*, struct instruction*);
};

struct instruction
{
    struct operation* operation;
    uint8_t opcode:6;
    enum format format;
    
    union
    {
        struct
        {
            /* R Format */
            uint8_t rformrs:5;
            uint8_t rformrt:5;
            uint8_t rformrd:5;
            uint8_t rformshift:5;
            uint8_t rformfunct:6;
        };

        struct
        {
            /* I Format */
            uint8_t iformrs:5;
            uint8_t iformrt:5;
            uint16_t iformimm;
        };

        struct
        {
            /* J Format */
            uint32_t jformaddress:26;
        };
    };
};

struct coprocessor0
{
    uint32_t vaddr;
    uint32_t status;
    uint32_t cause;
    uint32_t epc;
};

struct coprocessor1
{
    float f[32];
};

struct mipscpu
{
    uint32_t r[32];
    uint32_t pc;
    uint32_t hi;
    uint32_t lo;
    
    uint64_t memsize;
    uint8_t* memory;
    uint8_t running:1;

    struct coprocessor0 cp0;
    struct coprocessor1 fpu;

    struct operation rtype[44];
    struct operation othertype[44];
    struct operation ftype[1];
    struct operation cp0type[10];
};

void initmips(struct mipscpu* cpu);
void disassemble_mips(struct mipscpu* cpu);
void assemble_mips(char* text, char* out, uint64_t* size);
void emulate_cycle_mips(struct mipscpu* cpu);
void get_reg_name(uint8_t reg, char* buff);
void gen_exception(struct mipscpu* cpu, enum exception ex, uint32_t inv_addr);

#ifdef __cplusplus
}
#endif

#endif
