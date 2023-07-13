#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = *((int *)args); // 解析出 argc
  char **argv = (char **)args + 1; // 解析出 argv
  char **envp = ((char **)argv+argc+1);
  environ = envp;
  for(int i=0;i<argc;i++)
  {
    printf("%s\n",argv[i]);
  }
  exit(main(argc, argv, envp));
  assert(0);
}
