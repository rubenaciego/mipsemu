#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mipsopfuncs.h>
#include <mipscpu.h>

void get_reg_name(uint8_t reg, char* buff)
{
    if (reg == 0)
        strcpy(buff, "zero");
    else if (reg == 1)
        strcpy(buff, "at");
    else if (reg < 4)
        sprintf(buff, "v%u", reg - 2);
    else if (reg < 8)
        sprintf(buff, "a%u", reg - 4);
    else if (reg < 16)
        sprintf(buff, "t%u", reg - 8);
    else if (reg < 24)
        sprintf(buff, "s%u", reg - 16);
    else if (reg < 26)
        sprintf(buff, "t%u", reg - 16);
    else if (reg < 28)
        sprintf(buff, "k%u", reg - 26);
    else if (reg == 28)
        strcpy(buff, "gp");
    else if (reg == 29)
        strcpy(buff, "sp");
    else if (reg == 30)
        strcpy(buff, "fp");
    else if (reg == 31)
        strcpy(buff, "ra");
}

struct instruction get_next_instruction(struct mipscpu* cpu)
{
    struct instruction ins;
    uint32_t operation = *((uint32_t*)&cpu->memory[cpu->pc]);
    ins.opcode = get_opcode(operation);

    if (ins.opcode == 0)
    {
        ins.format = RFORMAT;
        ins.rformrt = (operation >> 16) & 31;
        ins.rformrs = (operation >> 21) & 31;
        ins.rformrd = (operation >> 11) & 31;
        ins.rformshift = (operation >> 6) & 31;
        ins.rformfunct = operation & 63;

        ins.operation = &cpu->rtype[ins.rformfunct];
    }
    else if (ins.opcode == 2 || ins.opcode == 3)
    {
        ins.format = JFORMAT;
        ins.jformaddress = (operation & 16777215) << 2;
        ins.jformaddress |= (operation >> 24) & 3;
        ins.jformaddress |= ((cpu->pc >> 28) & 15) << 28;
        
        ins.operation = &cpu->othertype[ins.opcode];
    }
    else if (ins.opcode == 16)
    {
        ins.format = RFORMAT;
        ins.rformrt = (operation >> 16) & 31;
        ins.rformrs = (operation >> 21) & 31;
        ins.rformrd = (operation >> 11) & 31;
        ins.rformshift = (operation >> 6) & 31;
        ins.rformfunct = operation & 63;

        ins.operation = &cpu->cp0type[ins.rformrs / 4];
    }
    else
    {
        ins.format = IFORMAT;
        ins.iformrt = (operation >> 16) & 31;
        ins.iformrs = (operation >> 21) & 31;
        ins.rformrd = (operation >> 11) & 31;
        ins.iformimm = operation & 0xffff;

        ins.operation = &cpu->othertype[ins.opcode];
    }

    return ins;
}

void initmips(struct mipscpu* cpu)
{
    memset(cpu, 0, sizeof(struct mipscpu));
    cpu->running = 1;
    cpu->r[29] = 0xfff;
    cpu->cp0.status = 0xff11;

    cpu->cp0type[0].pformat = RDRT;
    cpu->cp0type[0].func = mfc0;
    strcpy(cpu->cp0type[0].name, "MFC0");

    cpu->cp0type[1].pformat = RDRT;
    cpu->cp0type[1].func = mtc0;
    strcpy(cpu->cp0type[0].name, "MTC0");

    cpu->cp0type[4].pformat = NOTHING;
    cpu->cp0type[4].func = rfe;
    strcpy(cpu->cp0type[4].name, "RFE");

    cpu->rtype[0].pformat = RDRTSHAMT;
    cpu->rtype[0].func = sll0;
    strcpy(cpu->rtype[0].name, "SLL");

    cpu->rtype[2].pformat = RDRTSHAMT;
    cpu->rtype[2].func = srl2;
    strcpy(cpu->rtype[2].name, "SRL");

    cpu->rtype[3].pformat = RDRTSHAMT;
    cpu->rtype[3].func = sra3;
    strcpy(cpu->rtype[3].name, "SRA");

    cpu->rtype[4].pformat = RDRTRS;
    cpu->rtype[4].func = sllv4;
    strcpy(cpu->rtype[4].name, "SLLV");

    cpu->rtype[6].pformat = RDRTRS;
    cpu->rtype[6].func = srlv6;
    strcpy(cpu->rtype[6].name, "SRLV");

    cpu->rtype[7].pformat = RDRTRS;
    cpu->rtype[7].func = srav7;
    strcpy(cpu->rtype[7].name, "SRAV");

    cpu->rtype[8].pformat = RS;
    cpu->rtype[8].func = jr8;
    strcpy(cpu->rtype[8].name, "JR");

    cpu->rtype[9].pformat = RDRS;
    cpu->rtype[9].func = jalr9;
    strcpy(cpu->rtype[9].name, "JALR");

    cpu->rtype[9].pformat = RS;
    cpu->rtype[9].func = jalr9;
    strcpy(cpu->rtype[9].name, "JALR");

    cpu->rtype[12].pformat = NOTHING;
    cpu->rtype[12].func = syscall12;
    strcpy(cpu->rtype[12].name, "SYSCALL");

    cpu->rtype[16].pformat = RD;
    cpu->rtype[16].func = mfhi16;
    strcpy(cpu->rtype[16].name, "MFHI");

    cpu->rtype[17].pformat = RS;
    cpu->rtype[17].func = mthi17;
    strcpy(cpu->rtype[17].name, "MTHI");

    cpu->rtype[18].pformat = RD;
    cpu->rtype[18].func = mflo18;
    strcpy(cpu->rtype[18].name, "MFLO");

    cpu->rtype[19].pformat = RS;
    cpu->rtype[19].func = mtlo19;
    strcpy(cpu->rtype[19].name, "MTLO");

    cpu->rtype[24].pformat = RSRT;
    cpu->rtype[24].func = mult24;
    strcpy(cpu->rtype[24].name, "MULT");

    cpu->rtype[25].pformat = RSRT;
    cpu->rtype[25].func = multu25;
    strcpy(cpu->rtype[25].name, "MULTU");

    cpu->rtype[26].pformat = RSRT;
    cpu->rtype[26].func = div26;
    strcpy(cpu->rtype[26].name, "DIV");

    cpu->rtype[27].pformat = RSRT;
    cpu->rtype[27].func = divu27;
    strcpy(cpu->rtype[27].name, "DIVU");

    cpu->rtype[32].pformat = RDRSRT;
    cpu->rtype[32].func = add32;
    strcpy(cpu->rtype[32].name, "ADD");

    cpu->rtype[33].pformat = RDRSRT;
    cpu->rtype[33].func = addu33;
    strcpy(cpu->rtype[33].name, "ADDU");

    cpu->rtype[34].pformat = RDRSRT;
    cpu->rtype[34].func = sub34;
    strcpy(cpu->rtype[34].name, "SUB");

    cpu->rtype[35].pformat = RDRSRT;
    cpu->rtype[35].func = subu35;
    strcpy(cpu->rtype[35].name, "SUBU");

    cpu->rtype[36].pformat = RDRSRT;
    cpu->rtype[36].func = and36;
    strcpy(cpu->rtype[36].name, "AND");

    cpu->rtype[37].pformat = RDRSRT;
    cpu->rtype[37].func = or37;
    strcpy(cpu->rtype[37].name, "OR");

    cpu->rtype[38].pformat = RDRSRT;
    cpu->rtype[38].func = xor38;
    strcpy(cpu->rtype[38].name, "XOR");

    cpu->rtype[39].pformat = RDRSRT;
    cpu->rtype[39].func = nor39;
    strcpy(cpu->rtype[39].name, "NOR");

    cpu->rtype[42].pformat = RDRSRT;
    cpu->rtype[42].func = slt42;
    strcpy(cpu->rtype[42].name, "SLT");

    cpu->rtype[43].pformat = RDRSRT;
    cpu->rtype[43].func = sltu43;
    strcpy(cpu->rtype[43].name, "SLTU");


    cpu->othertype[2].pformat = ADDRESS;
    cpu->othertype[2].func = j2;
    strcpy(cpu->othertype[2].name, "J");

    cpu->othertype[3].pformat = ADDRESS;
    cpu->othertype[3].func = jal3;
    strcpy(cpu->othertype[3].name, "JAL");

    cpu->othertype[4].pformat = RSRTIMM;
    cpu->othertype[4].func = beq4;
    strcpy(cpu->othertype[4].name, "BEQ");

    cpu->othertype[5].pformat = RSRTIMM;
    cpu->othertype[5].func = bne5;
    strcpy(cpu->othertype[5].name, "BNE");

    cpu->othertype[6].pformat = RSIMM;
    cpu->othertype[6].func = blez6;
    strcpy(cpu->othertype[6].name, "BLEZ");

    cpu->othertype[7].pformat = RSIMM;
    cpu->othertype[7].func = bgtz7;
    strcpy(cpu->othertype[7].name, "BGTZ");

    cpu->othertype[8].pformat = RTRSIMM;
    cpu->othertype[8].func = addi8;
    strcpy(cpu->othertype[8].name, "ADDI");

    cpu->othertype[9].pformat = RTRSIMM;
    cpu->othertype[9].func = addiu9;
    strcpy(cpu->othertype[9].name, "ADDIU");

    cpu->othertype[10].pformat = RTRSIMM;
    cpu->othertype[10].func = slti10;
    strcpy(cpu->othertype[10].name, "SLTI");

    cpu->othertype[11].pformat = RTRSIMM;
    cpu->othertype[11].func = sltiu11;
    strcpy(cpu->othertype[11].name, "SLTIU");

    cpu->othertype[12].pformat = RTRSIMM;
    cpu->othertype[12].func = andi12;
    strcpy(cpu->othertype[12].name, "ANDI");

    cpu->othertype[13].pformat = RTRSIMM;
    cpu->othertype[13].func = ori13;
    strcpy(cpu->othertype[13].name, "ORI");

    cpu->othertype[14].pformat = RTRSIMM;
    cpu->othertype[14].func = xori14;
    strcpy(cpu->othertype[14].name, "XORI");

    cpu->othertype[15].pformat = RTIMM;
    cpu->othertype[15].func = lui15;
    strcpy(cpu->othertype[15].name, "LUI");

    cpu->othertype[32].pformat = RTIMMRS;
    cpu->othertype[32].func = lb32;
    strcpy(cpu->othertype[32].name, "LB");

    cpu->othertype[33].pformat = RTIMMRS;
    cpu->othertype[33].func = lh33;
    strcpy(cpu->othertype[33].name, "LH");

    cpu->othertype[35].pformat = RTIMMRS;
    cpu->othertype[35].func = lw35;
    strcpy(cpu->othertype[35].name, "LW");

    cpu->othertype[36].pformat = RTIMMRS;
    cpu->othertype[36].func = lbu36;
    strcpy(cpu->othertype[36].name, "LBU");

    cpu->othertype[37].pformat = RTIMMRS;
    cpu->othertype[37].func = lhu37;
    strcpy(cpu->othertype[37].name, "LHU");

    cpu->othertype[40].pformat = RTIMMRS;
    cpu->othertype[40].func = sb40;
    strcpy(cpu->othertype[40].name, "SB");

    cpu->othertype[41].pformat = RTIMMRS;
    cpu->othertype[41].func = sh41;
    strcpy(cpu->othertype[41].name, "SH");

    cpu->othertype[43].pformat = RTIMMRS;
    cpu->othertype[43].func = sw43;
    strcpy(cpu->othertype[43].name, "SW");
}

void disassemble_mips(struct mipscpu* cpu)
{
    struct instruction ins = get_next_instruction(cpu);

    printf("$%08x  ", cpu->pc);
    printf("%s", ins.operation->name);

    char rformrd[5];
    char rformrt[5];
    char rformrs[5];
    char iformrs[5];
    char iformrt[5];

    get_reg_name(ins.rformrd, rformrd);
    get_reg_name(ins.rformrt, rformrt);
    get_reg_name(ins.rformrs, rformrs);
    get_reg_name(ins.iformrs, iformrs);
    get_reg_name(ins.iformrt, iformrt);

    switch (ins.operation->pformat)
    {
        case ADDRESS:
            printf(" $%08x\n", ins.jformaddress);
        break;

        case RDRTSHAMT:
            printf(" $%s, $%s, %u\n", rformrd, rformrt, ins.rformshift);
        break;

        case RDRTRS:
            printf(" $%s, $%s, $%s\n", rformrd, rformrt, rformrs);
        break;

        case RDRT:
            printf(" $%s, $%s\n", rformrd, rformrt);
        break;

        case RDRSRT:
            printf(" $%s, $%s, $%s\n", rformrd, rformrs, rformrt);
        break;

        case RS:
            printf(" $%s\n", rformrs);
        break;

        case RDRS:
            printf(" $%s, $%s\n", rformrd, rformrs);
        break;

        case RD:
            printf(" $%s\n", rformrd);
        break;

        case RSRT:
            printf(" $%s, $%s\n", rformrs, rformrt);
        break;

        case RSRTIMM:
            printf(" $%s, $%s, %i\n", iformrs, iformrt, (int16_t)ins.iformimm);
        break;

        case RSIMM:
            printf(" $%s, %i\n", iformrs, (int16_t)ins.iformimm);
        break;

        case RTRSIMM:
            printf(" $%s, $%s, %i\n", iformrt, iformrs, (int16_t)ins.iformimm);
        break;

        case RTIMM:
            printf(" $%s, %i\n", iformrt, (int16_t)ins.iformimm);
        break;

        case RTIMMRS:
            printf(" $%s, %i($%s)\n", iformrt, (int16_t)ins.iformimm, iformrs);
        break;
    }
}

void assemble_mips(char* text, char* out, uint64_t* size)
{

}

void gen_exception(struct mipscpu* cpu, enum exception ex, uint32_t inv_addr)
{
    uint8_t temp = cpu->cp0.cause >> 8;
    cpu->cp0.cause = (temp << 8) | (ex << 2);

    if (ex == ADDRL || ex == ADDRS)
        cpu->cp0.vaddr = inv_addr;
    
    uint8_t int_enable = cpu->cp0.status & 1;
    uint8_t prev_status = (cpu->cp0.status >> 2) & 0b1111;
    uint8_t int_mask = cpu->cp0.status >> 8;

    cpu->cp0.status = (int_mask << 8) | int_enable | 2 | (prev_status << 2);

    cpu->cp0.epc = cpu->pc;
    cpu->pc = EXCP_ADDR;
}

void emulate_cycle_mips(struct mipscpu* cpu)
{
    disassemble_mips(cpu);

    struct instruction ins = get_next_instruction(cpu);
    ins.operation->func(cpu, &ins);
    cpu->r[0] = 0;

    printf("Registers:");
    char regname[5];

    for (int i = 0; i < 32; i++)
    {
        if (i % 4 == 0)
            printf("\n");

        get_reg_name(i, regname);        
        printf("$%s = $%08x\t", regname, cpu->r[i]);

        if (i != 0)
            printf("\t");
    }

    printf("\n$PC = $%08x\t\t$HI = $%08x\t\t$LO = $%08x\n\n", cpu->pc, cpu->hi, cpu->lo);
}
