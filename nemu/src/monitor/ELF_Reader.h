#include <assert.h>
#include <elf.h>
#include <stdio.h>
// #define p_flag 0

typedef struct {
  char name[30];     /* Symbol name (string tbl index) */
  Elf32_Addr start;  /* Symbol value */
  Elf32_Word length; /* Symbol size */
  // unsigned char	st_info;		/* Symbol type and binding */
  // unsigned char	st_other;		/* Symbol visibility */
  // Elf32_Section	st_shndx;		/* Section index */
} Elf_func;

int get_sym_func(const char *elf_src, Elf_func *sym_func) {
  Elf32_Ehdr head;
  FILE *fp;
  char shstr_s[800], str_s[800];
  // section
  Elf32_Shdr se[40];
  // symbol
  Elf32_Sym sym[100];
  int n_sym = 0;
  int n_sym_func = 0;
  fp = fopen(elf_src, "r");
  int ret = fread(&head, sizeof(Elf32_Ehdr), 1, fp);
  assert(ret == 1);

  fseek(fp, head.e_shoff, SEEK_SET);
  ret = fread(se, sizeof(Elf32_Shdr), head.e_shnum, fp);
  assert(ret == head.e_shnum);

  // try to get section's real name in shstr,store in shstr_s
  fseek(fp, se[head.e_shstrndx].sh_offset, SEEK_SET);
  ret = fread(shstr_s, sizeof(char), se[head.e_shstrndx].sh_size / sizeof(char),
              fp);
  assert(ret == se[head.e_shstrndx].sh_size / sizeof(char));
#ifdef p_flag
  printf(
      "[NR] %20s type  flags %10s offset  size sh_link  sh_info sh_addralign "
      "sh_entsize\n",
      "name", "addr");
#endif
  for (int i = 0; i < head.e_shnum; i++) {
#ifdef p_flag
    printf("[%d] %20s %5d %5d %10x %5x %5d %5d %5d %5d %5d\n", i,
           shstr_s + se[i].sh_name, se[i].sh_type, se[i].sh_flags,
           se[i].sh_addr, se[i].sh_offset, se[i].sh_size, se[i].sh_link,
           se[i].sh_info, se[i].sh_addralign, se[i].sh_entsize);
#endif
    // get str
    if (se[i].sh_type == SHT_STRTAB && i != head.e_shstrndx) {
      fseek(fp, se[i].sh_offset, SEEK_SET);
      ret = fread(str_s, sizeof(char), se[i].sh_size / sizeof(char), fp);
      assert(ret == se[i].sh_size / sizeof(char));
    }
    // get sym
    if (se[i].sh_type == SHT_SYMTAB) {
      fseek(fp, se[i].sh_offset, SEEK_SET);
      n_sym =
          fread(sym, sizeof(Elf32_Sym), se[i].sh_size / sizeof(Elf32_Sym), fp);
    }
  }
#ifdef p_flag
  printf("%3s%20s%10s%5s%5s%5s\n", "[NR]", "Name", "Value", "Size", "Type",
         "Bind");
#endif
  for (int i = 0; i < n_sym; i++) {
#ifdef p_flag
    printf("%3d%20s%10x%5d%5d%5d\n", i, str_s + sym[i].st_name, sym[i].st_value,
           sym[i].st_size, ELF32_ST_TYPE(sym[i].st_info),
           ELF32_ST_BIND(sym[i].st_info));
#endif
    if (ELF32_ST_TYPE(sym[i].st_info) == STT_FUNC) {
      // sym_func[n_sym_func] = sym[i];
      strcpy(sym_func[n_sym_func].name, str_s + sym[i].st_name);
      sym_func[n_sym_func].start = sym[i].st_value;
      sym_func[n_sym_func].length = sym[i].st_size;
      // name_func[n_sym_func] = str_s + sym[i].st_name;
      n_sym_func++;
    }
  }
  fclose(fp);
  return n_sym_func;
}

// int main() {
//   Elf32_Sym sym_func[100];

//   int n = get_sym_func("build/add-riscv32-nemu.elf", sym_func);
//   printf("\n%d\n", n);
// }