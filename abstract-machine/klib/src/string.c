#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t count = 0;
  for (const char *p = s; *p != '\0'; p++) {
    count++;
  }
  return count;
}

char *strcpy(char *dst, const char *src) {
  char *q = dst;
  for (const char *p = src; *p != '\0'; p++) {
    *q = *p;
    q++;
  }
  *q='\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *q=dst;
  for (const char *p = src; *p != '\0'&&n; p++) {
    *q = *p;
    q++;
    n--;
  }
  while(n>0){
    *q='\0';
    q++;
    n--;
  }
  *q='\0';
  return dst;
}

char *strcat(char *dst, const char *src) {
  char *s;

  for (s = dst; *s != '\0'; ++s)
    ;
  for (; (*s = *src) != '\0'; ++s, ++src)
    ;
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  for (; (*s1 == *s2) && (*s1 != '\0'); s1++, s2++)
    ;
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  for (; (n > 0) && (*s1 == *s2) && (*s1 != '\0'); s1++, s2++, n--)
    ;
  if (n == 0) return 0;
  return *s1 - *s2;
}

void *memset(void *s, int c, size_t n) {
  const unsigned char uc = c;
  unsigned char *su;

  for (su = s; 0 < n; ++su, --n) *su = uc;
  return (s);
}

void *memmove(void *dst, const void *src, size_t n) {
  char *sc1;
  const char *sc2;

  sc1 = dst;
  sc2 = src;
  if (sc2 > sc1 && sc1 < sc2 + n)
    for (sc1 += n, sc2 += n; 0 < n; --n) *--sc1 = *--sc2;
  else
    for (; 0 < n; --n) *sc1++ = *sc2++;
  return (dst);
}

void *memcpy(void *out, const void *in, size_t n) {
  char *su1;
  const char *su2;

  for (su1 = out, su2 = in; 0 < n; ++su1, ++su2, --n) *su1 = *su2;
  return (out);
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *su1, *su2;
  for (su1 = s1, su2 = s2; 0 < n; ++su1, ++su2, --n)
    if (*su1 != *su2) return ((*su1 < *su2) ? -1 : +1);
  return 0;
}

#endif
