#include <isa.h>
#include <string.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/vaddr.h>
//对给定字符串求值
word_t expr(char *e, bool *success);

//token's type
enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM,TK_16,TK_DEREF
  /* TODO: Add more token types */
  	
};

//每一条规则对应的正则表达式和token's type
static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"^0x[0-9]+",TK_16},
  {"^\\$(\\S)+",'$'},


  {"[0-9]+", TK_NUM},    // num:257

  {"\\+", '+'},         // plus
  {"\\-", '-'},         // subtract
  {"\\*", '*'},         // multiply
  {"\\/", '/'},         // divide
  {"\\(", '('},         // bracket_left
  {"\\)", ')'},         // bracket_right

  {"==", TK_EQ},        // equal

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}
//存放一个识别出来的token
typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[100] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          //ignore space
          case TK_NOTYPE: 
            break;
          //ignore ==
          case TK_EQ: 
            break;
          case '$':
          //不管16进制还是10进制，存下来就行了
          case TK_16:
          case TK_NUM: 
            tokens[nr_token].type = rules[i].token_type;
            //to avoid overflow
            if (substr_len>30)
               substr_len=30;
            char temp=substr_start[substr_len];
            substr_start[substr_len]='\0';
            strcpy(tokens[nr_token].str,substr_start);
            substr_start[substr_len]=temp;
            nr_token++;
            break;
          default: 
            tokens[nr_token++].type=rules[i].token_type;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
  	  }
  }

  return true;
}
//找到主操作符
int find_main(int p,int q)
{
  int left_p=0;
  int ans=0;
  for(int i=p;i<=q;i++)
  {
    if(tokens[i].type=='(')left_p++;
    if(tokens[i].type==')')left_p--;
    if(left_p!=0)continue;
    if(tokens[i].type=='+'||tokens[i].type=='-')
      return i;  
    if(tokens[i].type=='*'||tokens[i].type=='/')
      ans=i;
  }
  return ans;
}
//检查[p,q]段是否被括号包裹
static bool check_parentheses(int p,int q){
  if(tokens[p].type=='('&&tokens[q].type==')'){
    int check=1;
    for(int i=p+1;i<q;i++){
      if(tokens[i].type=='(')check++;
      if(tokens[i].type==')')check--;
      if(check == 0 )return false;
    }
    //printf("has cut off!\n");
    return true;
  }
  return false;
}


//计算 [p,q]之间的token所对应的值
word_t eval(int p,int q, bool *success)
{
  if (*success==false)return 0;
  //printf("%d %d\n",p,q);
  if(p>q||p<0){
    *success=false;
    return 0;
  }
  else if(p==q){
    if (tokens[p].type == TK_NUM)
    return atoi(tokens[p].str);
    int x;
    sscanf(tokens[p].str,"%x",&x);
    return x;
  }
  //开头是 TK_DEREF * 的 
  else if(tokens[p].type == TK_DEREF){
    int address = eval(p+1,q,success);
    return vaddr_read(address,1);
  }
  else if(check_parentheses(p,q)){
    return eval(p+1,q-1,success);
  }
  else{
    int op_pos=find_main(p,q);
    //if(op_pos)printf("%d\n",op_pos);
    int val1 = eval(p, op_pos - 1,success);
    int val2 = eval(op_pos+1,q,success);
    switch(tokens[op_pos].type){
      case '+':return val1 + val2;
      case '-':return val1 - val2;
      case '*':return val1 * val2;
      case '/':
      if(val2==0){
        printf("x/0!!");
        *success=false;
        return 0;
      }
      return val1 / val2;
      default :
        printf("unkoown op!");
        *success=false;
    }  
  }
  return 0;
}

void del_token(int obj){
  if(obj<0||obj>=nr_token)return ;
  for(int i=obj+1;i<nr_token;i++){
    tokens[i-1] = tokens[i];
  }
  nr_token--;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  //printf("make_token[%d] has finished!\n",nr_token);
  
  //处理一下 $t1 以及 *0x123434 之类的
  for(int i=0;i<nr_token;i++){
    if(tokens[i].type == '$'){
      bool flag = true;
      int val = isa_reg_str2val(tokens[i].str+1,&flag);
      if(!flag)
        printf("%s can't find\n",tokens[i].str); 
      tokens[i].type = TK_16;
      sprintf(tokens[i].str,"%x",val);
    }
    if(tokens[i].type == '*' &&
        (i == 0 || (tokens[i-1].type<'a'&&tokens[i-1].type!=')') ) ){
      tokens[i].type = TK_DEREF;
    }
  }

  *success = true;
  /* TODO: Insert codes to evaluate the expression. */
  return eval(0,nr_token-1,success);	
  //return 0;
}
