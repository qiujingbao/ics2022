#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t len = 0;
	char *t = (char*) s;
	while (*t++ != '\0') {
		len += 1;
	}
  //panic("Not implemented");
	return len;
}

char *strcpy(char *dst, const char *src) {
  size_t i;
	for (i = 0; src[i] != '\0'; i++)
		dst[i] = src[i];
	dst[i] = '\0';
	return dst;
	//panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
	size_t i = 0;
	for (i = 0; i < n && src[i] != '\0'; i++)
		dst[i] = src[i];
	for (; i < n; i++)
		dst[i] = '\0';
	return dst;
  //panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  size_t i = 0;
	size_t len = strlen(dst);
	for (i = 0; src[i] != '\0'; i++)
		dst[len+i] = src[i];
	dst[len+i] = '\0';
	return dst;
	//panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
	int flag = 1;
	while(*s2) {
		flag = *s1 - *s2;
		if(flag) {
			break;
		}
		++s1;
		++s2;
	}
	return flag;
  //panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
	int flag = 1;
	while(n-- && *s2) {
		flag = *s1 - *s2;
		if (flag) {
			break;
		}
		++s1;
		++s2;
	}
	return flag;
  //panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  size_t i = 0;
	char *t = (char*) s;
	for (i = 0; i < n; i++)
		t[i] = (char)c;
	return s;
	//panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  char *p = (char*)dst;
	char *q = (char*)src;
	if (src >= dst) {
		while (n--) {
			*p++ = *q++;
		}
	}
	else {
		p += n;
		q += n;
		while (n--) {
			*p-- = *q--;
		}
	}
	return dst;
	//panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
	char* p = (char*)out;
	char* q = (char*)in;
	while (n--) {
		*p++ = *q++;
	}
	return out;
  //panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  char *p = (char*)s1;
	char *q = (char*)s2;
	int flag = 0;
	while (n--) {
		flag = *p - *q;
		if (!flag) break;
		++p;
		++q;
	}
	return flag;
	//panic("Not implemented");
}

#endif
