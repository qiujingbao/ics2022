/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <common.h>
#include "memory/vaddr.h"
#define MAX_TOKEN 64
enum
{
  TK_NOTYPE = 256,
  TK_EQ,
  TK_LEFT = 0,
  TK_RIGHT = 1,
  // 双目运算符
  TK_PLUS = 2,   // +
  TK_SUB = 3,    // -
  TK_MUT = 4,    // *
  TK_DIV = 5,    // /
  TK_EQUAL = 6,  //==
  TK_NEQUAL = 7, //!=
  TK_AND = 8,    //&&

  // 单目运算符
  TK_NEG = 9,    // -
  TK_DEREF = 10, // *

  TK_NUM = 11,
  TK_OX = 12, // 0x2222
  TK_REG = 13 // reg
  /* TODO: Add more token types */

};
// 993 / 652*762
static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {
    {"[(]", TK_LEFT},
    {"[)]", TK_RIGHT},
    {"[0]{1}[xX]{1}[0-9a-zA-Z]+", TK_OX},
    {"[0-9]+", TK_NUM},

    {"[$][a-z0-9A-Z]+", TK_REG},
    {" +", TK_NOTYPE}, // spaces

    {"[!]{1}[=]{1}", TK_NEQUAL},
    {"[=]{2}", TK_EQUAL},
    {"[&]{2}", TK_AND},
    {"[+]", TK_PLUS},

    {"[-]", TK_SUB},
    {"[*]", TK_MUT},
    {"[/]", TK_DIV},
    {"\\+", '+'},  // plus
    {"==", TK_EQ}, // equal
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[MAX_TOKEN] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (nr_token >= MAX_TOKEN)
      {
        return false;
      }

      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //   i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type)
        {
        case TK_NUM:
          tokens[nr_token].type = TK_NUM;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          break;
        case TK_OX:
          tokens[nr_token].type = TK_OX;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          break;
        case TK_REG:
          tokens[nr_token].type = TK_REG;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          break;
        case TK_NOTYPE:
          break;
        default:
          tokens[nr_token].type = rules[i].token_type;
        }
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
// 找到i与j之间与i匹配的括号
int match_bu(int i, int j)
{
  int x = 0;
  for (; i <= j; i++)
  {
    if (tokens[i].type == TK_LEFT)
      x++;
    else if (tokens[i].type == TK_RIGHT)
      x--;
    if (x == 0)
      return i;
  }
  return -1;
}
bool check_parentheses(int i, int j)
{
  if (tokens[i++].type != TK_LEFT || tokens[j--].type != TK_RIGHT)
    return false;
  for (; i < j; i++)
  {
    if (tokens[i].type == TK_LEFT)
    {
      int x = match_bu(i, j);
      if (x == -1)
        return false;
    }
  }
  return true;
}
int get_mainop(int i, int j)
{
  int single_op = -1;
  int pos_main = -1;
  int level4 = -1;
  int pos = -1;
  for (; i <= j; i++)
  {
    if (tokens[i].type == TK_NUM)
      continue;

    if (tokens[i].type == TK_LEFT)
    {
      int res = match_bu(i, j);
      if (res == -1)
        return -1;
      i = res;
    }

    if (tokens[i].type == TK_DIV || tokens[i].type == TK_MUT)
    {
      pos = i;
    }
    else if (tokens[i].type == TK_PLUS || tokens[i].type == TK_SUB)
    {
      pos_main = i;
    }
    else if (tokens[i].type == TK_NEG || tokens[i].type == TK_DEREF)
    {
      single_op = i;
    }
    else if (tokens[i].type == TK_EQUAL || tokens[i].type == TK_NEQUAL)
    {
      level4 = i;
    }
  }
  if (single_op != -1)
    return single_op;
  else if (pos_main != -1)
    return pos_main;
  else if (pos != -1)
    return pos;
  else
    return level4;
}
int str2int(char *str)
{
  int res = 0;
  int i = 0;
  int tem = 0;
  while (*(str + i) != '\0')
  {
    tem = *(str + i) - '0';
    res *= 10;
    res += tem;
  }
  return tem;
}
uint64_t jisuan(uint64_t x, uint64_t y, int op)
{
  switch (op)
  {
  case TK_PLUS:
    return x + y;
  case TK_SUB:
    return x - y;
  case TK_MUT:
    return x * y;
  case TK_DIV:
  {
    if (y == 0)
    {
      printf("div=0\n");
      return -1;
    }
    // assert(val2 != 0);
    return x / y;
  }
  case TK_NEQUAL:
    return x!=y;
  case TK_EQUAL:
    return x==y;
  default:
    assert(0);
  }
  return 0;
}
uint64_t eval(int start, int end)
{
  if (start > end)
    return -1;
  else if (start == end)
  {
    switch (tokens[start].type)
    {
    case TK_REG:
    {
      bool su = false;
      char *s = (tokens[start].str);
      uint64_t x = isa_reg_str2val(s + 1, &su);
      if (su == true)
        return x;
      else
        return 0;
    }
    break;
    case TK_OX:
    {
      uint64_t x = 0;
      sscanf(tokens[start].str, "%lx", &x);
      return x;
    }
    break;
    case TK_NUM:
      return atoi(tokens[start].str);
      break;
    default:
      break;
    }
  }

  else if (check_parentheses(start, end) == true)
  {
    return eval(start + 1, end - 1);
  }
  else
  {
    int op = get_mainop(start, end);
    if (op == -1)
    {
      panic("error!");
    }

    if ((tokens[op].type == TK_NEG || tokens[op].type == TK_DEREF) && op == start)
    {
      if (tokens[op].type == TK_DEREF)
        return vaddr_read(eval(op + 1, end), 8);
      else if (tokens[op].type == TK_NEG)
        return -eval(op + 1, end);
    }
    else
    {
      uint64_t val1 = 0;
      uint64_t val2 = 0;
      if (tokens[op].type == TK_NEG || tokens[op].type == TK_DEREF)
      {
        val2 = eval(op, end);
        op = get_mainop(start, op - 1);
        val1 = eval(start, op - 1);
      }
      else
      {
        val1 = eval(start, op - 1);
        val2 = eval(op + 1, end);
      }

      return jisuan(val1, val2, tokens[op].type);
    }
  }
  return 0;
}

word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }
  for (int i = 0; i < nr_token; i++)
  {
    if (tokens[i].type == TK_MUT && (i == 0 || tokens[i - 1].type != TK_NUM))
    {
      tokens[i].type = TK_DEREF;
    }
    if (tokens[i].type == TK_SUB && (i == 0 || tokens[i - 1].type != TK_NUM))
    {
      tokens[i].type = TK_NEG;
    }
  }
  uint64_t res = eval(0, nr_token - 1);
  memset(tokens, '\0', sizeof(tokens));
  *success = true;
  return res;
}