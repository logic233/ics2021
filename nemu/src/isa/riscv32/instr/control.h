def_EHelper(beq) {
  if(*dsrc1==*dsrc2){
    rtl_j(s, cpu.pc+id_dest->imm);
  }
}
def_EHelper(bne) {
    if(*dsrc1!=*dsrc2){
    rtl_j(s, cpu.pc+id_dest->imm);
  }
}
def_EHelper(blt) {
    if((sword_t)*dsrc1<(sword_t)*dsrc2){
    rtl_j(s, cpu.pc+id_dest->imm);
  }
}
def_EHelper(bltu) {
    if(*dsrc1<*dsrc2){
    rtl_j(s, cpu.pc+id_dest->imm);
  }
}
def_EHelper(bge) {
    if((sword_t)*dsrc1>=(sword_t)*dsrc2){
    rtl_j(s, cpu.pc+id_dest->imm);
  }
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