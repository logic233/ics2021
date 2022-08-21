#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int isDigit(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}
int sprintf(char *out, const char *fmt, ...) {
   int count = 0;
//     char c;
    char *s;
    int n;
    
//     int index = 0;
//     int ret = 2;
    
    char buf[65];
    char digit[16];
//     int num = 0;
//     int len = 0;
    
    memset(buf, 0, sizeof(buf));
    memset(digit, 0, sizeof(digit));

    va_list ap;
    
    va_start(ap, fmt);
    
    while(*fmt != '\0')
    {
        // printf("*fmt=[%c]\n", *fmt);
        if(*fmt == '%')
        {
            fmt++;
            switch(*fmt)
         {
                case 'd': /*整型*/
                {
                        n = va_arg(ap, int);
                        if(n < 0)
                        {
                            *out = '-';
                            out++;
                            n = -n;
                        }
                        itoa(n, buf);
                        memcpy(out, buf, strlen(buf));
                        out += strlen(buf);
                        break;
                }    
                // case 'c': /*字符型*/
                // {
                //         c = va_arg(ap, int);
                //         *out = c;
                //         out++;
                //         break;
                // }
                // case 'x': /*16进制*/
                // {
                //         n = va_arg(ap, int);
                //         xtoa(n, buf);
                //         memcpy(out, buf, strlen(buf));
                //         out += strlen(buf);
                //         break;
                // }
                case 's': /*字符串*/
                {
                        s = va_arg(ap, char *);
                        memcpy(out, s, strlen(s));
                        out += strlen(s);
                        break;
                }
                // case '%': /*输出%*/
                // {
                //     *out = '%';
                //     out++;
                //     break;
                // }
                // case '0': /*位不足的左补0*/
                // {
                //         index = 0;
                //         num = 0;
                //         memset(digit, 0, sizeof(digit));
                        
                //         while(1)
                //         {
                //                 fmt++;
                //                 ret = isDigit(*fmt);
                //                 if(ret == 1) //是数字
                //                 {
                //                         digit[index] = *fmt;
                //                         index++;
                //                 }
                //                 else
                //                 {
                //                         num = atoi(digit);
                //                         break;
                //                 }
                //         }
                //         switch(*fmt)
                //      {
                //                 case 'd': /*整型*/
                //                 {
                //                         n = va_arg(ap, int);
                //                         if(n < 0)
                //                         {
                //                             *out = '-';
                //                             out++;
                //                             n = -n;
                //                         }    
                //                         itoa(n, buf);
                //                         len = strlen(buf);
                //                         if(len >= num)
                //                         {
                //                                 memcpy(out, buf, strlen(buf));
                //                                 out += strlen(buf);
                //                         }
                //                         else
                //                         {
                //                                 memset(out, '0', num-len);
                //                                 out += num-len;
                //                                 memcpy(out, buf, strlen(buf));
                //                                 out += strlen(buf);
                //                         }
                //                         break;
                //                 }    
                //                 case 'x': /*16进制*/
                //                 {
                //                         n = va_arg(ap, int);
                //                         xtoa(n, buf);
                //                         len = strlen(buf);
                //                         if(len >= num)
                //                         {
                //                                 memcpy(out, buf, len);
                //                                 out += len;
                //                         }            
                //                         else
                //                         {
                //                                 memset(out, '0', num-len);
                //                                 out += num-len;
                //                                 memcpy(out, buf, len);
                //                                 out += len;
                //                         }
                //                         break;
                //                 }
                //                 case 's': /*字符串*/
                //                 {
                //                         s = va_arg(ap, char *);
                //                         len = strlen(s);
                //                         if(len >= num)
                //                         {
                //                                 memcpy(out, s, strlen(s));
                //                                 out += strlen(s);
                //                         }
                //                         else
                //                         {
                //                                 memset(out, '0', num-len);
                //                                 out += num-len;
                //                                 memcpy(out, s, strlen(s));
                //                                 out += strlen(s);
                //                         }
                //                         break;
                //                 }
                //                 default:
                //                         break;
                //         }
                // }
                default:
                        break;
            }
        }
        else
        {
            *out = *fmt;
            out++;
        }
        fmt++;
    }
    *out = '\0';

    va_end(ap);

    return count;
}


int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
