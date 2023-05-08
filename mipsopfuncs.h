#ifndef MIPS_OPFUNCS_H
#define MIPS_OPFUNCS_H

#ifdef __cplusplus
extern "C" {
#endif

struct mipscpu;
struct instruction;

void rfe(struct mipscpu* cpu, struct instruction* ins);
void mtc0(struct mipscpu* cpu, struct instruction* ins);
void mfc0(struct mipscpu* cpu, struct instruction* ins);
void sll0(struct mipscpu* cpu, struct instruction* ins);
void srl2(struct mipscpu* cpu, struct instruction* ins);
void sra3(struct mipscpu* cpu, struct instruction* ins);
void sllv4(struct mipscpu* cpu, struct instruction* ins);
void srlv6(struct mipscpu* cpu, struct instruction* ins);
void srav7(struct mipscpu* cpu, struct instruction* ins);
void jr8(struct mipscpu* cpu, struct instruction* ins);
void jalr9(struct mipscpu* cpu, struct instruction* ins);
void syscall12(struct mipscpu* cpu, struct instruction* ins);
void mfhi16(struct mipscpu* cpu, struct instruction* ins);
void mthi17(struct mipscpu* cpu, struct instruction* ins);
void mflo18(struct mipscpu* cpu, struct instruction* ins);
void mtlo19(struct mipscpu* cpu, struct instruction* ins);
void mult24(struct mipscpu* cpu, struct instruction* ins);
void multu25(struct mipscpu* cpu, struct instruction* ins);
void div26(struct mipscpu* cpu, struct instruction* ins);
void divu27(struct mipscpu* cpu, struct instruction* ins);
void add32(struct mipscpu* cpu, struct instruction* ins);
void addu33(struct mipscpu* cpu, struct instruction* ins);
void sub34(struct mipscpu* cpu, struct instruction* ins);
void subu35(struct mipscpu* cpu, struct instruction* ins);
void and36(struct mipscpu* cpu, struct instruction* ins);
void or37(struct mipscpu* cpu, struct instruction* ins);
void xor38(struct mipscpu* cpu, struct instruction* ins);
void nor39(struct mipscpu* cpu, struct instruction* ins);
void slt42(struct mipscpu* cpu, struct instruction* ins);
void sltu43(struct mipscpu* cpu, struct instruction* ins);
void j2(struct mipscpu* cpu, struct instruction* ins);
void jal3(struct mipscpu* cpu, struct instruction* ins);
void beq4(struct mipscpu* cpu, struct instruction* ins);
void bne5(struct mipscpu* cpu, struct instruction* ins);
void blez6(struct mipscpu* cpu, struct instruction* ins);
void bgtz7(struct mipscpu* cpu, struct instruction* ins);
void addi8(struct mipscpu* cpu, struct instruction* ins);
void addiu9(struct mipscpu* cpu, struct instruction* ins);
void slti10(struct mipscpu* cpu, struct instruction* ins);
void sltiu11(struct mipscpu* cpu, struct instruction* ins);
void andi12(struct mipscpu* cpu, struct instruction* ins);
void ori13(struct mipscpu* cpu, struct instruction* ins);
void xori14(struct mipscpu* cpu, struct instruction* ins);
void lui15(struct mipscpu* cpu, struct instruction* ins);
void lb32(struct mipscpu* cpu, struct instruction* ins);
void lh33(struct mipscpu* cpu, struct instruction* ins);
void lw35(struct mipscpu* cpu, struct instruction* ins);
void lbu36(struct mipscpu* cpu, struct instruction* ins);
void lhu37(struct mipscpu* cpu, struct instruction* ins);
void sb40(struct mipscpu* cpu, struct instruction* ins);
void sh41(struct mipscpu* cpu, struct instruction* ins);
void sw43(struct mipscpu* cpu, struct instruction* ins);

#ifdef __cplusplus
}
#endif

#endif
