#include "sdb.h"
#define NR_WP 5
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }
  head = NULL;
  free_ = wp_pool;
}
WP *new_wp()
{
  assert(free_);
  struct watchpoint *res;
  res = free_;
  free_ = free_->next;
  res->next = NULL;

  if (head == NULL)
    head = res;
  else
  {
    res->next = head;
    head = res;
  }
  return res;
}
void free_wp(WP *wp)
{
  WP *t = NULL;
  if (head->NO == wp->NO)
  {
    head = head->next;
    return;
  }
  t = head->next;
  WP *pre = head;
  while (t)
  {
    if (t->NO == wp->NO)
    {
      pre->next = t->next;
      break;
    }
    t = t->next;
    pre = pre->next;
  }
  wp->next = free_;
  free_ = wp;
  return;
}
void display_watchpoint()
{
  WP *p = head;
  printf("num\twhat\n");
  while (p)
  {
    printf("%d\t%s\n", p->NO, p->exp);
    p = p->next;
  }
}
void delete_watchpoint(int no)
{
  WP *p = head;
  while (p)
  {
    if (p->NO == no)
    {
      free_wp(p);
      return;
    }
    p = p->next;
  }
  return;
}
void insert_watchpoint(char *exp)
{
  bool success = false;
  uint64_t v = expr(exp, &success);
  if (success == true)
  {
    WP *n = new_wp();
    assert(n);
    n->val = v;
    strcpy(n->exp, exp);
  }
  return;
}

WP *re_mut_watchpoint()
{
  WP *t = head;
  while (t)
  {
    bool su = false;
    uint64_t v = expr(t->exp, &su);
    if (v != t->val && su == true)
    {
      t->old_val = t->val;
      t->val = v;
      return t;
    }
    t=t->next;
  }
  return NULL;
}