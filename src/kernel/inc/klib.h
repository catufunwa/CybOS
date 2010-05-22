/******************************************************************************
 *
 *  File        : klib.h
 *  Description : Kernel library functions
 *
 *****************************************************************************/
#ifndef __KLIB_H__
#define __KLIB_H__

  /* Va_list stuff for do_printf */
  typedef char *va_list;

  #define __va_size(type) \
        (((sizeof(type)+sizeof(long)-1)/sizeof(long)) * sizeof(long))

  #define va_start(ap, last) \
        ((ap)=(va_list)&(last)+__va_size(last))

  #define va_arg(ap, type) \
        (*(type *)((ap) += __va_size(type), (ap) - __va_size(type)))

  #define va_end(ap) ((void)0)

  typedef int (*fnptr)(char c, void **helper);    /* do_printf helper */


  // NULL is null. period.
  #define NULL    0



  int do_printf (const char *fmt, va_list args, fnptr fn, void *ptr);

  char *strcpy (char *dst, const char *src);
  char *strncpy (char *dst, const char *src, int count);
  int strlen (const char *str);
  int strcmp (const char *str1, const char *str2);
  int strncmp (const char *str1, const char *str2, int count);
  char *strstr (const char *str1, const char *str2);

  void *memcpy (void *dst_ptr, const void *src_ptr, int count);
  void *memset (void *dst, int val, int count);
  void *memmove (void *dest, void *src, int count);



  int bsf (int bit_field);                    // bit scan forward
  int btr (int bit_field, int bit_index);     // Bit test & reset
  int bts (int bit_field, int bit_index);     // Bit test & set

#endif    // __KLIB_H__
