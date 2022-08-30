#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}
 void itoa(unsigned int n, char * buf)
{
   int i;
   if (n < 10) {
     buf[0] = n + '0';
     buf[1] = '\0';
     return;
   }
   itoa(n / 10, buf);
   for (i = 0; buf[i] != '\0'; i++)
     ;
   buf[i] = (n % 10) + '0';
   buf[i + 1] = '\0';
}
 void ltoa(unsigned long n, char * buf)
{
   int i;
   if (n < 10) {
     buf[0] = n + '0';
     buf[1] = '\0';
     return;
   }
   itoa(n / 10, buf);
   for (i = 0; buf[i] != '\0'; i++)
     ;
   buf[i] = (n % 10) + '0';
   buf[i + 1] = '\0';
}
 void xtoa(unsigned int n, char * buf)
{
   int i;

   if (n < 16) {
     if (n < 10) {
       buf[0] = n + '0';
     } else {
       buf[0] = n - 10 + 'a';
     }
     buf[1] = '\0';
     return;
   }
   xtoa(n / 16, buf);

   for (i = 0; buf[i] != '\0'; i++)
     ;

   if ((n % 16) < 10) {
     buf[i] = (n % 16) + '0';
   } else {
     buf[i] = (n % 16) - 10 + 'a';
   }
   buf[i + 1] = '\0';
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  panic("Not implemented");
#endif
  return NULL;
}

void free(void *ptr) {
}

#endif
