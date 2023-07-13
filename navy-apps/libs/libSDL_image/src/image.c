#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load(const char *filename) {
  printf("image load:%s\n",filename);
  FILE *fp=fopen(filename,"r");
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *buf = malloc(size);
  fread(buf, 1, size, fp);
  SDL_Surface *s = STBIMG_LoadFromMemory(buf, size);
  fclose(fp);
  free(buf);
  return s;
}


int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
