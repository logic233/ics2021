def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}
def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm);
  rtl_addi(s, ddest, ddest,cpu.pc);
}
def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1,id_src2->imm);
}

def_EHelper(jal) {
  rtl_addi(s, ddest, &cpu.pc,id_src1->imm);
  rtl_jr(s,ddest);
  rtl_addi(s, ddest, &cpu.pc,4);
}
def_EHelper(jalr) {
  rtl_addi(s, s0, &cpu.pc,4);

  rtl_addi(s, ddest, dsrc1, id_src2->imm);
  rtl_andi(s,ddest,ddest,~1);
  rtl_jr(s,ddest);
  rtl_mv(s, ddest, s0);
}