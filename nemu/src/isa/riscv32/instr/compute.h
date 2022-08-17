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
def_EHelper(sltiu) {
  if(*dsrc1 < id_src2->imm)
  rtl_li(s, ddest, 1);
  else
  rtl_li(s, ddest, 0);
}
def_EHelper(slli) {
  if(id_src2->imm<32)
  //id_src2->imm is 0000000[shamt]
  //so shamt = id_src2->imm 
  rtl_slli(s, ddest, dsrc1,id_src2->imm);
}
def_EHelper(srli) {
  if(id_src2->imm<32)
  //id_src2->imm is 0000000[shamt]
  //so shamt = id_src2->imm 
  rtl_srli(s, ddest, dsrc1,id_src2->imm);
}
def_EHelper(srai) {
  if(id_src2->imm<1024+32)
  //id_src2->imm is 0100000[shamt]
  //so shamt = id_src2->imm -1024
  rtl_srai(s, ddest, dsrc1,id_src2->imm-1024);
}

def_EHelper(add) {
  rtl_add(s, ddest, dsrc1,dsrc2);
}
def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1,dsrc2);
}




def_EHelper(slt) {
  if ((sword_t)*dsrc1 < (sword_t)*dsrc2)
    rtl_li(s, ddest, 1);
  else
    rtl_li(s, ddest, 0);
}

def_EHelper(sltu) {
  if (*dsrc1 < *dsrc2)
    rtl_li(s, ddest, 1);
  else
    rtl_li(s, ddest, 0);
}
def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1,(sword_t)id_src2->imm);
}
def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1,(sword_t)id_src2->imm);
}
def_EHelper(sll) {
  rtl_slli(s, ddest, dsrc1,*dsrc2%64);
}
def_EHelper(and) {
  rtl_andi(s, ddest, dsrc1,(sword_t)*dsrc2);
}
def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1,dsrc2);
}
def_EHelper(sra) {
  rtl_srai(s, ddest, dsrc1,*dsrc2%64);
}
def_EHelper(srl) {
  rtl_srli(s, ddest, dsrc1,*dsrc2%64);
}
def_EHelper(or) {
  rtl_or(s, ddest, dsrc1,dsrc2);
}


def_EHelper(mul) {
  rtl_mulu_lo(s, ddest, dsrc1,dsrc2);
}
def_EHelper(mulh) {
  rtl_muls_hi(s, ddest, dsrc1,dsrc2);
}
def_EHelper(div) {
  rtl_divs_q(s, ddest, dsrc1,dsrc2);
}
def_EHelper(rem) {
  rtl_divs_r(s, ddest, dsrc1,dsrc2);
}