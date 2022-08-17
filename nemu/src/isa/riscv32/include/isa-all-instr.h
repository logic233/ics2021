#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f)                                                          \
  f(lui) f(auipc) f(jal) f(jalr) f(lw) f(lh) f(lb) f(lhu) f(lbu) f(sw) f(sb) f(inv) f(nemu_trap) f(addi) \
      f(add) f(sub) f(slt) f(sltu) f(beq) f(bne) f(blt) f(sltiu) f(xori) f(xor) f(or)  \
          f(sh) f(slli) f(srli)f(srai) f(andi) f(bge) f(mul) f(div) f(sll) f(rem)
def_all_EXEC_ID();
