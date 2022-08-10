#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static void gen_rand_expr() {
  // buf[0] = '\0';
  int choose = rand();
  int num;
  char *op_list[4]={"+","-","*","/"};
  // printf("%d\n",choose);
  if(strlen(buf)>40){
    choose = 0;
  }
 switch (choose%5) {
   case 0:
    num = rand() % 100;
     sprintf(buf + strlen(buf), "%d", num);
     return;
   case 1:
     sprintf(buf + strlen(buf), "(");
     gen_rand_expr();
     sprintf(buf + strlen(buf), ")");
     return;
   default:
     gen_rand_expr();
     sprintf(buf + strlen(buf), "%s", op_list[rand()%4]);
     gen_rand_expr();
     return;
  }
  return;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    memset(buf,0,sizeof(buf));
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
