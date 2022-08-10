#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/vaddr.h>

enum
{
  TK_NOTYPE = 256,
  TK_EQ,
  TK_DECNUM,
  TK_HEXNUM,
  TK_REG,
  TK_DEREF
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE}, // spaces
    {"\\+", '+'},      // plus
    {"==", TK_EQ},     // equal
    {"-", '-'},
    {"\\*", '*'},
    {"/", '/'},
    {"\\(", '('},
    {"\\)", ')'},
    {"\\$[a-z0-9]+",TK_REG},
    {"0x[a-f0-9]+", TK_HEXNUM},
    {"[0-9]+", TK_DECNUM},
    
};

int isOP(int token_type){
  int op_list[20] = {'+', '-', '*', '/', TK_EQ,TK_DEREF};
  for (int i = 0; i < 20; i++) {
    if (token_type == op_list[i]) return 1;
  }
  return 0;
}
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

typedef struct token {
  int type;
  char str[32];
} Token;
#define MAX_TOKENS 320
static Token tokens[MAX_TOKENS] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  //init
  nr_token = 0;
  for (int i = 0; i < MAX_TOKENS; i++)
    memset(tokens[nr_token].str, 0, sizeof(tokens[nr_token].str));

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
        case TK_NOTYPE:
          break;
        case TK_HEXNUM:
        case TK_REG:
        case TK_DECNUM:
          if (substr_len > 30)
            substr_len = 30;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
        default:
          tokens[nr_token].type = rules[i].token_type;
          nr_token++;
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

bool check_parentheses(int p, int q) {
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    int left = 1;
    for (int i = p + 1; i < q; i++) {
      if (tokens[i].type == '(') left++;
      if (tokens[i].type == ')') left--;
      if (left <= 0) return false;
    }
    return left == 1;
  }
  return false;
}

word_t eval(int p, int q) {
  // printf("eval[%d %d]\n",p,q);
  if (p > q) {
    /* Bad expression */
    assert(0);
  } else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
      int val;
      char val_s[10];
      switch (tokens[p].type)
      {
      case TK_DECNUM:
        sscanf(tokens[p].str, "%d", &val);
        break;
      case TK_HEXNUM:
        sscanf(tokens[p].str, "%x", &val);
        break;
      case TK_REG:
        sscanf(tokens[p].str, "$%s", val_s);
        if(val_s[0]=='0')return 0;
        bool success;
        val = isa_reg_str2val(val_s, &success);
        if(!success){
          panic("reg can't find\n");
          assert(0);
        }
        break;
      default:
        break;
      }
      return val;
  } else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    //TODO!!
    return eval(p + 1, q - 1);
  } else {
    // find major OP
    int op = p;
    int major[400] = {};
    int left=0;
    memset(major,99,sizeof(int)*400);
    major['+'] = 10;
    major['-'] = 10;
    major['*'] = 20;
    major['/'] = 20;
    major[TK_EQ] = 5;
    major[TK_DEREF] = 40;
    for (int i = p; i <= q; i++) {
      int type = tokens[i].type;
      if (type == '(') left+=1;
      if (type == ')') left-=1;
      if (left == 0) {
        if (major[type] < major[tokens[op].type]) op = i;
        if (major[type] == major[tokens[op].type]) {
          // DEREF right is higher. so just ignore
          if (type == TK_DEREF) continue;
          op = i;
        }
      }
    }
    //
    if(tokens[op].type==TK_DEREF){
      return vaddr_read(eval(op+1,q),4);
    }
    // printf("[OP]%d",op);
    int val1 = eval(p, op - 1);
    int val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+':
        return val1 + val2;
      case '-':
        return val1 - val2;
      case '*':
        return val1 * val2;
      case '/':
        return val1 / val2;
      case TK_EQ:
        return val1 == val2 ;
      default:
        assert(0);
    }
  }

  return 0;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == '*' && (i == 0 || isOP(tokens[i - 1].type))) {
      tokens[i].type = TK_DEREF;
    }
  }

  // for(int i=0;i<nr_token;i++){
  //   printf("%d %s\n",tokens[i].type,tokens[i].str);
  // }
  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token - 1);

  return 0;
}
