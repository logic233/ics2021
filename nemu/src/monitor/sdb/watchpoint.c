#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  int value;
  char exp[120];
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

void wp_display() {
  puts("NO\texp\told_value");
  for( WP *wp_x=head;wp_x!=NULL;wp_x=wp_x->next){
    printf("%d\t%s\t%u/ ",wp_x->NO,wp_x->exp,wp_x->value);
    printf(FMT_WORD,wp_x->value);
    puts("");
  }
  return;
}

/* TODO: Implement the functionality of watchpoint */
void insert_wp(char* exp) {
  WP *wp_x;
  if(free_==NULL ){
    panic("too many wp");
    assert(0);
  }
  wp_x = free_;
  free_ = free_->next;
  wp_x->next = head;
  head = wp_x;
  //init wp;
  strcpy(wp_x->exp , exp);

  bool sucess;
  int ans;
  ans = expr(exp, &sucess);
  if (!sucess)
    panic("Wrong EXPR");
  else 
    wp_x->value = ans;
  
}
void free_wp(int no) {
   WP *wp_x;
   if (head == NULL) {
     printf("pls check NO\n");
     return;
   }

  if(head->NO == no){
    wp_x = head;
    head = wp_x->next;
    wp_x->next = free_;
    free_ = wp_x;
    return;
  }
  WP *wp_pre = head;
  for(wp_x=head->next;wp_x!=NULL;wp_x=wp_x->next){
    if(wp_x->NO == no){
      wp_pre->next = wp_x->next;
      wp_x->next = free_;
      free_ = wp_x;
      return ;
    }
    wp_pre = wp_pre -> next;
  }
  printf("pls check NO\n");
  return ;
}

bool scan_wp() {
  bool sucess;
  int ans;
  bool ret=false;
  for( WP *wp_x=head;wp_x!=NULL;wp_x=wp_x->next){
      ans = expr(wp_x->exp, &sucess);
      if(ans!=wp_x->value){
          ret=true;
          printf("\nwatchpoint %d: %s\n\nold: %u/ ",wp_x->NO,wp_x->exp,wp_x->value);
          printf(FMT_WORD,wp_x->value);
          puts("");
          printf("new: %u/ ",ans);
          printf(FMT_WORD,ans);
          puts("");
      }
  }
  return ret;
}