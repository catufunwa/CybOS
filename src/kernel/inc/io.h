/******************************************************************************
 *
 *  File        : io.h
 *  Description : Low level input output function defines.
 *
 *****************************************************************************/
#ifndef __IO_H__
#define __IO_H__

  #include "ktype.h"

  void outb (Uint32 addr, Uint8  b);
  void outw (Uint32 addr, Uint16 w);
  void outl (Uint32 addr, Uint32 l);

  Uint8  inb (Uint32 addr);
  Uint16 inw (Uint32 addr);
  Uint32 inl (Uint32 addr);

#endif //__IO_H__
