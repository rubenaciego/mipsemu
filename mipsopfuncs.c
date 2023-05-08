#include <stdio.h>
#include <stdlib.h>
#include <mipscpu.h>
#include <mipsopfuncs.h>

uint32_t signext32(uint32_t val, uint8_t bits)
{
	if ((val >> (bits - 1)) & 1)
		val |= 0xffffffff << (32 - bits);

	return val;
}

void rfe(struct mipscpu* cpu, struct instruction* ins)
{
	uint8_t int_mask = cpu->cp0.status >> 8;
	uint8_t prev_status = (cpu->cp0.status >> 2) & 0b1111; 
	
	cpu->cp0.status = (int_mask << 8) | prev_status;

	cpu->pc = cpu->cp0.epc;
}

void mtc0(struct mipscpu* cpu, struct instruction* ins)
{
	switch (ins->rformrt)
	{
		case 12:
			cpu->cp0.status = cpu->r[ins->rformrd];
		break;

		case 13:
			cpu->cp0.cause = cpu->r[ins->rformrd];
		break;

		case 14:
			cpu->cp0.epc = cpu->r[ins->rformrd];
		break;
	}

	cpu->pc += INS_SIZE;
}

void mfc0(struct mipscpu* cpu, struct instruction* ins)
{
	switch (ins->rformrt)
	{
		case 12:
			cpu->r[ins->rformrd] = cpu->cp0.status;
		break;

		case 13:
			cpu->r[ins->rformrd] = cpu->cp0.cause;
		break;

		case 14:
			cpu->r[ins->rformrd] = cpu->cp0.epc;
		break;
	}

	cpu->pc += INS_SIZE;
}

void sll0(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrt] << ins->rformshift;
	cpu->pc += INS_SIZE;
}

void srl2(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrt] >> ins->rformshift;
	cpu->pc += INS_SIZE;
}

void sra3(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrt] >> ins->rformshift;
	
	if ((cpu->r[ins->rformrt] >> 31) & 1)
		cpu->r[ins->rformrd] |= 0xffffffff << (32 - ins->rformshift);
	
	cpu->pc += INS_SIZE;
}

void sllv4(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrt] << cpu->r[ins->rformrs];
	cpu->pc += INS_SIZE;
}

void srlv6(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrt] >> cpu->r[ins->rformrs];
	cpu->pc += INS_SIZE;
}

void srav7(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrt] >> cpu->r[ins->rformrs];
	
	if ((cpu->r[ins->rformrt] >> 31) & 1)
		cpu->r[ins->rformrd] |= 0xffffffff << (32 - cpu->r[ins->rformrs]);
	
	cpu->pc += INS_SIZE;
}

void jr8(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->pc = cpu->r[ins->rformrs];
}

void jalr9(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->pc + INS_SIZE;
	cpu->pc = cpu->r[ins->rformrs];
}

void syscall12(struct mipscpu* cpu, struct instruction* ins)
{
	gen_exception(cpu, SYSCALL, 0);
}

void mfhi16(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->hi;
	cpu->pc += INS_SIZE;
}

void mthi17(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->hi = cpu->r[ins->rformrs];
	cpu->pc += INS_SIZE;
}

void mflo18(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->lo;
	cpu->pc += INS_SIZE;
}

void mtlo19(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->lo = cpu->r[ins->rformrs];
	cpu->pc += INS_SIZE;
}

void mult24(struct mipscpu* cpu, struct instruction* ins)
{
	int64_t result = (int64_t)cpu->r[ins->rformrs] * (int64_t)cpu->r[ins->rformrt];
	
	cpu->hi = (result >> 32) & 0xffffffff;
	cpu->lo = result & 0xffffffff;

	cpu->pc += INS_SIZE;
}

void multu25(struct mipscpu* cpu, struct instruction* ins)
{
	uint64_t result = (uint64_t)cpu->r[ins->rformrs] * (uint64_t)cpu->r[ins->rformrt];
	
	cpu->hi = (result >> 32) & 0xffffffff;
	cpu->lo = result & 0xffffffff;

	cpu->pc += INS_SIZE;
}

void div26(struct mipscpu* cpu, struct instruction* ins)
{	
	cpu->hi = ((int32_t)cpu->r[ins->rformrs] / (int32_t)cpu->r[ins->rformrt]) & 0xffffffff;
	cpu->lo = (int32_t)cpu->r[ins->rformrs] % (int32_t)cpu->r[ins->rformrt];

	if (cpu->r[ins->rformrt] == 0)
		gen_exception(cpu, DBUS, 0);
	else
		cpu->pc += INS_SIZE;
}

void divu27(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->hi = (cpu->r[ins->rformrs] / cpu->r[ins->rformrt]) & 0xffffffff;
	cpu->lo = cpu->r[ins->rformrs] % cpu->r[ins->rformrt];

	if (cpu->r[ins->rformrt] == 0)
		gen_exception(cpu, DBUS, 0);
	else
		cpu->pc += INS_SIZE;
}

void add32(struct mipscpu* cpu, struct instruction* ins)
{
	int64_t res64 = (int64_t)cpu->r[ins->rformrs] + (int64_t)cpu->r[ins->rformrt];
	int32_t res32 = (int32_t)cpu->r[ins->rformrs] + (int32_t)cpu->r[ins->rformrt];
	cpu->r[ins->rformrd] = res32;

	if (res64 != res32)
		gen_exception(cpu, OVF, 0);
	else
		cpu->pc += INS_SIZE;
}

void addu33(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrs] + cpu->r[ins->rformrt];
	cpu->pc += INS_SIZE;
}

void sub34(struct mipscpu* cpu, struct instruction* ins)
{
	int64_t res64 = (int64_t)cpu->r[ins->rformrs] - (int64_t)cpu->r[ins->rformrt];
	int32_t res32 = (int32_t)cpu->r[ins->rformrs] - (int32_t)cpu->r[ins->rformrt];
	cpu->r[ins->rformrd] = res32;

	if (res64 != res32)
		gen_exception(cpu, OVF, 0);
	else
		cpu->pc += INS_SIZE;
}

void subu35(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrs] - cpu->r[ins->rformrt];
	cpu->pc += INS_SIZE;
}

void and36(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrs] & cpu->r[ins->rformrt];
	cpu->pc += INS_SIZE;
}

void or37(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrs] | cpu->r[ins->rformrt];
	cpu->pc += INS_SIZE;
}

void xor38(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrs] ^ cpu->r[ins->rformrt];
	cpu->pc += INS_SIZE;
}

void nor39(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = !(cpu->r[ins->rformrs] | cpu->r[ins->rformrt]);
	cpu->pc += INS_SIZE;
}

void slt42(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = (int32_t)cpu->r[ins->rformrs] < (int32_t)cpu->r[ins->rformrt];
	cpu->pc += INS_SIZE;
}

void sltu43(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->rformrd] = cpu->r[ins->rformrs] < cpu->r[ins->rformrt];
	cpu->pc += INS_SIZE;
}

void j2(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->pc = ((cpu->pc + INS_SIZE) & 4026531840) | (ins->jformaddress << 2);
}

void jal3(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[31] = cpu->pc + INS_SIZE;
	cpu->pc = ((cpu->pc + INS_SIZE) & 4026531840) | (ins->jformaddress << 2);
}

void beq4(struct mipscpu* cpu, struct instruction* ins)
{
	if (cpu->r[ins->iformrs] == cpu->r[ins->iformrt])
		cpu->pc += signext32(ins->iformimm << 2, 18);
	
	cpu->pc += INS_SIZE;
}

void bne5(struct mipscpu* cpu, struct instruction* ins)
{
	if (cpu->r[ins->iformrs] != cpu->r[ins->iformrt])
		cpu->pc += signext32(ins->iformimm << 2, 18);
	
	cpu->pc += INS_SIZE;
}

void blez6(struct mipscpu* cpu, struct instruction* ins)
{
	if ((int32_t)cpu->r[ins->iformrs] <= 0)
		cpu->pc += signext32(ins->iformimm << 2, 18);
	
	cpu->pc += INS_SIZE;
}

void bgtz7(struct mipscpu* cpu, struct instruction* ins)
{
	if ((int32_t)cpu->r[ins->iformrs] > 0)
		cpu->pc += signext32(ins->iformimm << 2, 18);
	
	cpu->pc += INS_SIZE;
}

void addi8(struct mipscpu* cpu, struct instruction* ins)
{
	int64_t res64 = (int64_t)cpu->r[ins->iformrs] + (int64_t)signext32(ins->iformimm, 16);
	int32_t res32 = (int32_t)cpu->r[ins->iformrs] + (int32_t)signext32(ins->iformimm, 16);
	cpu->r[ins->iformrt] = res32;

	if (res64 != res32)
		gen_exception(cpu, OVF, 0);
	else
		cpu->pc += INS_SIZE;
}

void addiu9(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = cpu->r[ins->iformrs] + signext32(ins->iformimm, 16);
	cpu->pc += INS_SIZE;
}

void slti10(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = (int32_t)cpu->r[ins->iformrs] < (int32_t)signext32(ins->iformimm, 16);
	cpu->pc += INS_SIZE;
}

void sltiu11(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = cpu->r[ins->iformrs] < signext32(ins->iformimm, 16);
	cpu->pc += INS_SIZE;
}

void andi12(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = cpu->r[ins->iformrs] & (uint32_t)ins->iformimm;
	cpu->pc += INS_SIZE;
}

void ori13(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = cpu->r[ins->iformrs] | (uint32_t)ins->iformimm;
	cpu->pc += INS_SIZE;
}

void xori14(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = cpu->r[ins->iformrs] ^ (uint32_t)ins->iformimm;
	cpu->pc += INS_SIZE;
}

void lui15(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = (((uint32_t)ins->iformimm) << 16);
	cpu->pc += INS_SIZE;
}

void lb32(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = signext32(cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16)], 8);
	cpu->pc += INS_SIZE;
}

void lh33(struct mipscpu* cpu, struct instruction* ins)
{
	uint8_t byte1 = cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16)];
	uint8_t byte2 = cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 1];
	cpu->r[ins->iformrt] = signext32((byte2 << 8) | byte1, 16);
	cpu->pc += INS_SIZE;
}

void lw35(struct mipscpu* cpu, struct instruction* ins)
{
	uint8_t byte1 = cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16)];
	uint8_t byte2 = cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 1];
	uint8_t byte3 = cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 2];
	uint8_t byte4 = cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 3];
	cpu->r[ins->iformrt] = (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
	cpu->pc += INS_SIZE;
}

void lbu36(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->r[ins->iformrt] = (uint32_t)cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16)];;
	cpu->pc += INS_SIZE;
}

void lhu37(struct mipscpu* cpu, struct instruction* ins)
{
	uint8_t byte1 = cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16)];
	uint8_t byte2 = cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 1];
	cpu->r[ins->iformrt] = (uint32_t)((byte2 << 8) | byte1);
	cpu->pc += INS_SIZE;
}

void sb40(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16)] = cpu->r[ins->iformrt] & 0xff;
	cpu->pc += INS_SIZE;
}

void sh41(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16)] = cpu->r[ins->iformrt] & 0xff;
	cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 1] = (cpu->r[ins->iformrt] >> 8) & 0xff;
	cpu->pc += INS_SIZE;
}

void sw43(struct mipscpu* cpu, struct instruction* ins)
{
	cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16)] = cpu->r[ins->iformrt] & 0xff;
	cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 1] = (cpu->r[ins->iformrt] >> 8) & 0xff;
	cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 2] = (cpu->r[ins->iformrt] >> 16) & 0xff;
	cpu->memory[cpu->r[ins->iformrs] + signext32(ins->iformimm, 16) + 3] = (cpu->r[ins->iformrt] >> 24) & 0xff;
	cpu->pc += INS_SIZE;
}
