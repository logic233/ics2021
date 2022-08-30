#ifdef CONFIG_FTRACE
int check_func_byElf(rtlreg_t dest);
#endif

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
def_EHelper(bgeu) {
    if(*dsrc1>=*dsrc2){
    rtl_j(s, cpu.pc+id_dest->imm);
  }
}


def_EHelper(jal) {
  rtl_addi(s, s1, &cpu.pc,id_src1->imm);

  rtl_jr(s,s1);
  rtl_addi(s, ddest, &cpu.pc,4);
  #ifdef CONFIG_FTRACE
  //call function like : jal x1,offset
  if(s->isa.instr.j.rd == 1)
  check_func_byElf(*s1);
#endif
}

def_EHelper(jalr) {
  rtl_addi(s, s0, &cpu.pc,4);

  rtl_addi(s, s1, dsrc1, id_src2->imm);
  rtl_andi(s,s1,s1,~1);

  rtl_jr(s,s1);
  rtl_mv(s, ddest, s0);
#ifdef CONFIG_FTRACE
  //ret <=> jalr x0,0(x1)
  //only this situaiton need FTRACE
  if(s->isa.instr.j.rd == 1||(s->isa.instr.i.rd==0&&id_src2->imm==0&&s->isa.instr.i.rs1==1))
    check_func_byElf(*s1);
#endif
}