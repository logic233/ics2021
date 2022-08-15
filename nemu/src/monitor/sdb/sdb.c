#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/vaddr.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

void wp_display();
void insert_wp(char* exp);
void free_wp(int no);
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  int step;
  if (args == NULL || sscanf(args, "%d", &step) != 1)
    step = 1;
  cpu_exec(step);
  return 0;
}

static int cmd_help(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);
static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands.", cmd_help},
    {"c", "Continue the execution of the program.", cmd_c},
    {"q", "Exit NEMU.", cmd_q},
    {"si", "Continue the execution of the program with [N] steps and pause, defalut N = 1.", cmd_si},
    {"info", "Print state of Regs by \033[4m`info r`\033[0m, print info of watchpoints by \033[4m`info w`\033[0m.", cmd_info},
    {"x", "Use like \033[4m`x N EXPR`\033[0m. Compute the EXPR. From result, output N consecutive 4 bytes in HEX form.", cmd_x},
    {"p", "Compute the EXPR by \033[4m`p EXPR`\033[0m.", cmd_p},
    {"w", "Set watchpoint by \033[4m`w EXPR`\033[0m, and pause the program when EXPR changes.", cmd_w},
    {"d", "Delete No.[N] watchpoint by \033[4m`d N`\033[0m.", cmd_d},
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_info(char *args) {
  char c = '\0';
  sscanf(args, "%c", &c);

  switch (c) {
  case 'r':
    isa_reg_display();
    break;
  case 'w':
    wp_display();
    break;
  default:
    printf("Unknown command ' info %s'\n", args);
  }
  return 0;
}
static int cmd_x(char *args) {
  int N;
  vaddr_t addr;
  char *arg = strtok(NULL, " ");
  sscanf(arg, "%d", &N);
  arg = strtok(NULL, " ");

  bool sucess;
  int ans;
  ans = expr(arg, &sucess);
  if (!sucess)
    panic("Wrong EXPR");
  else {
    addr = ans;
  }

  while(N) {
    // if NEMU is 32bit . Luck! Every addr contain 4 BYTES!!
    printf(FMT_WORD, addr);
    printf(" : ");
    printf(FMT_WORD, vaddr_read(addr, 4));
    puts(" ");
    addr += 4;
    N--;
  }

  return 0;
};

static int cmd_p(char *args) {
  bool sucess;
  int ans;
  ans = expr(args, &sucess);
  if (!sucess)
    printf("Wrong EXPR\n");
  else {
    printf("%u ", ans);
    printf(FMT_WORD, ans);
    puts("");
  }
  return 0;
  };

static int cmd_w(char *args) {
  insert_wp(args);
  return 0;
};

static int cmd_d(char *args) {
  int no;
  int check = sscanf(args,"%d",&no);
  if(check==1)
  free_wp(no);
  else
    printf("check args\n");
  return 0;
};

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  // if (is_batch_mode) {
  //   cmd_c(NULL);
  //   return;
  // }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
