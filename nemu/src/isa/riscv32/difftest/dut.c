#include <cpu/difftest.h>
#include <isa.h>

#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (ref_r->pc != cpu.pc) {
          printf("%s is not same.\nDUT:0x%x\nREF:0x%x\n", "pc",cpu.pc,ref_r->pc);
    return false;
  }
  for (int i = 0; i < 32; i++) {
    if (cpu.gpr[i]._32 != ref_r->gpr[i]._32) {
      printf("%s is not same.\nDUT:0x%x\nREF:0x%x\n", reg_name(i,1),cpu.gpr[i]._32,
             ref_r->gpr[i]._32);
      return false;
    }
  }
  return true;
}

void isa_difftest_attach() {}
