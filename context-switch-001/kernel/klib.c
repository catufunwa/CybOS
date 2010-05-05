/******************************************************************************
 *
 *  File        : klib.c
 *  Description : stdlib functions for kernel use
 *
 *****************************************************************************/
#include "klib.h"

// ========================================================================================
// bit scan forward
int bsf (int bit_field) {
  int bit_index;

  // If bit_field is empty, unpredictable results is returned. Always return with -1.
  if (bit_field == 0) return -1;

  // Could be done in C, but we do it in asm..
  __asm__ __volatile__ ("bsfl %%eax, %%ebx" : "=b" (bit_index) : "a" (bit_field));
  return bit_index;
}

// Bit test & reset
int btr (int bit_field, int bit_index) {
  int new_bit_field;

  // Could be done in C, but we do it in asm..
  __asm__ __volatile__ ("btr %%ebx, %%eax" : "=a" (new_bit_field) : "a" (bit_field), "b" (bit_index));
  return new_bit_field;
}

// Bit test & set
int bts (int bit_field, int bit_index) {
  int new_bit_field;

  // Could be done in C, but we do it in asm..
  __asm__ __volatile__ ("bts %%ebx, %%eax" : "=a" (new_bit_field) : "a" (bit_field), "b" (bit_index));
  return new_bit_field;
}


// ======================================================================
char *strncpy (char *dst, const char *src, int count) {
  char *ret_val = dst;

  for ( ; (*src != '\0') && (count != 0); count--) *dst++ = *src++;
  *dst = '\0';
  return ret_val;
}

// ======================================================================
void *memcpy (void *dst_ptr, const void *src_ptr, int count) {
  void *ret_val = dst_ptr;
  const char *src = (const char *)src_ptr;
  char *dst=(char *)dst_ptr;

  for ( ; count!=0; count--) *dst++=*src++;
  return ret_val;
}

// ======================================================================
void *memset (void *dst, int val, int count) {
  char *temp;

  for (temp=(char *)dst; count!=0; count--) *temp++=val;
  return dst;
}

// ======================================================================
// Much faster memmove than when written in plain C. This one takes care
// of a odd byte, an odd word and does the rest in straight dwords.
void *memmove (void *dest, void *src, int count) {
  __asm__ __volatile__(
                        "    cld\n"
                        "    shr %%ecx\n"
                        "    jnc 1f\n"
                        "    movsb\n"
                        "1:  shr %%ecx\n"
                        "    jnc 2f\n"
                        "    movsw\n"
                        "2:  rep\n"
                        "    movsl\n"
                       :
                       : "c"(count), "D"(dest), "S"(src)
        );
  return dest;
}



// ======================================================================
int strlen (const char *str) {
  int ret_val;

  for (ret_val=0; *str!='\0'; str++) ret_val++;
  return ret_val;
}

// ======================================================================
int strcmp (const char *str1, const char *str2) {
  while ((*str2 != '\0') && (*str1 == *str2)) {
    str1++;
    str2++;
  }
  return *str1 - *str2;
}



// ======================================================================
  // Flags user in processing format string
  #define PR_LJ   0x01    // Left Justify
  #define PR_CA   0x02    // Casing (A..F instead of a..f)
  #define PR_SG   0x04    // Signed conversion (%d vs %u)
  #define PR_32   0x08    // Long (32bit)
  #define PR_16   0x10    // Short (16bit)
  #define PR_WS   0x20    // PR_SG set and num < 0
  #define PR_LZ   0x40    // Pad left with '0' instead of ' '
  #define PR_FP   0x80    // Far pointers

  #define PR_BUFLEN  16


int do_printf (const char *fmt, va_list args, fnptr fn, void *ptr) {
	unsigned flags, actual_wd, count, given_wd;
	unsigned char *where, buf[PR_BUFLEN];
	unsigned char state, radix;
	long num;

	state = flags = count = given_wd = 0;
/* begin scanning format specifier list */
	for(; *fmt; fmt++)
	{
		switch(state)
		{
/* STATE 0: AWAITING % */
		case 0:
			if(*fmt != '%')	/* not %... */
			{
				fn(*fmt, &ptr);	/* ...just echo it */
				count++;
				break;
			}
/* found %, get next char and advance state to check if next char is a flag */
			state++;
			fmt++;
			/* FALL THROUGH */
/* STATE 1: AWAITING FLAGS (%-0) */
		case 1:
			if(*fmt == '%')	/* %% */
			{
				fn(*fmt, &ptr);
				count++;
				state = flags = given_wd = 0;
				break;
			}
			if(*fmt == '-')
			{
				if(flags & PR_LJ)/* %-- is illegal */
					state = flags = given_wd = 0;
				else
					flags |= PR_LJ;
				break;
			}
/* not a flag char: advance state to check if it's field width */
			state++;
/* check now for '%0...' */
			if(*fmt == '0')
			{
				flags |= PR_LZ;
				fmt++;
			}
			/* FALL THROUGH */
/* STATE 2: AWAITING (NUMERIC) FIELD WIDTH */
		case 2:
			if(*fmt >= '0' && *fmt <= '9')
			{
				given_wd = 10 * given_wd +
					(*fmt - '0');
				break;
			}
/* not field width: advance state to check if it's a modifier */
			state++;
			/* FALL THROUGH */
/* STATE 3: AWAITING MODIFIER CHARS (FNlh) */
		case 3:
			if(*fmt == 'F')
			{
				flags |= PR_FP;
				break;
			}
			if(*fmt == 'N')
				break;
			if(*fmt == 'l')
			{
				flags |= PR_32;
				break;
			}
			if(*fmt == 'h')
			{
				flags |= PR_16;
				break;
			}
/* not modifier: advance state to check if it's a conversion char */
			state++;
			/* FALL THROUGH */
/* STATE 4: AWAITING CONVERSION CHARS (Xxpndiuocs) */
		case 4:
			where = buf + PR_BUFLEN - 1;
			*where = '\0';
			switch(*fmt)
			{
			case 'X':
				flags |= PR_CA;
				/* FALL THROUGH */
/* xxx - far pointers (%Fp, %Fn) not yet supported */
			case 'x':
			case 'p':
			case 'n':
				radix = 16;
				goto DO_NUM;
			case 'd':
			case 'i':
				flags |= PR_SG;
				/* FALL THROUGH */
			case 'u':
				radix = 10;
				goto DO_NUM;
			case 'o':
				radix = 8;
/* load the value to be printed. l=long=32 bits: */
DO_NUM:				if(flags & PR_32)
                                  num = va_arg(args, unsigned long);
/* h=short=16 bits (signed or unsigned) */
				else if(flags & PR_16)
				{
					if(flags & PR_SG)
						num = va_arg(args, short);
					else
						num = va_arg(args, unsigned short);
				}
/* no h nor l: sizeof(int) bits (signed or unsigned) */
				else
				{
					if(flags & PR_SG)
						num = va_arg(args, int);
					else
						num = va_arg(args, unsigned int);
				}
/* take care of sign */
				if(flags & PR_SG)
				{
					if(num < 0)
					{
						flags |= PR_WS;
						num = -num;
					}
				}
/* convert binary to octal/decimal/hex ASCII
OK, I found my mistake. The math here is _always_ unsigned */
				do
				{
					unsigned long temp;

					temp = (unsigned long)num % radix;
					where--;
					if(temp < 10)
						*where = (unsigned char)(temp + '0');
					else if(flags & PR_CA)
						*where = (unsigned char)(temp - 10 + 'A');
					else
						*where = (unsigned char)(temp - 10 + 'a');
					num = (unsigned long)num / radix;
				}
				while(num != 0);
				goto EMIT;
			case 'c':
/* disallow pad-left-with-zeroes for %c */
				flags &= ~PR_LZ;
				where--;
				*where = (unsigned char)va_arg(args,
					unsigned char);
				actual_wd = 1;
				goto EMIT2;
			case 's':
/* disallow pad-left-with-zeroes for %s */
				flags &= ~PR_LZ;
				where = va_arg(args, unsigned char *);
EMIT:
				actual_wd = (unsigned int)strlen((const char *)where);
				if(flags & PR_WS)
					actual_wd++;
/* if we pad left with ZEROES, do the sign now */
				if((flags & (PR_WS | PR_LZ)) ==
					(PR_WS | PR_LZ))
				{
					fn('-', &ptr);
					count++;
				}
/* pad on left with spaces or zeroes (for right justify) */
EMIT2:				if((flags & PR_LJ) == 0)
				{
					while(given_wd > actual_wd)
					{
						fn(flags & PR_LZ ?
							'0' : ' ', &ptr);
						count++;
						given_wd--;
					}
				}
/* if we pad left with SPACES, do the sign now */
				if((flags & (PR_WS | PR_LZ)) == PR_WS)
				{
					fn('-', &ptr);
					count++;
				}
/* emit string/char/converted number */
				while(*where != '\0')
				{
					fn(*where++, &ptr);
					count++;
				}
/* pad on right with spaces (for left justify) */
				if(given_wd < actual_wd)
					given_wd = 0;
				else given_wd -= actual_wd;
				for(; given_wd; given_wd--)
				{
					fn(' ', &ptr);
					count++;
				}
				break;
			default:
				break;
			}
		default:
			state = flags = given_wd = 0;
			break;
		}
	}
	return count;
}
