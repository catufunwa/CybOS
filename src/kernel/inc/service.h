/******************************************************************************
 *
 *  File        : service.h
 *  Description : Service Routine like DOS 0x21 interrupt header and defines
 *
 *****************************************************************************/
#ifndef __SERVICE_H__
#define __SERVICE_H__

  #include "idt.h"
  #include "console.h"


  // Syscall defines

  #define SYS_NULL                        0
  #define SYS_CONSOLE                     1
      #define SYS_CONSOLE_CREATE               0
      #define SYS_CONSOLE_DESTROY              1
  #define SYS_CONWRITE                       2
  #define SYS_CONREAD                        3
  #define SYS_CONFLUSH                       4

  #define SYS_FORK                       10


  int service_interrupt (int sysnr, int p1, int p2, int p3, int p4, int p5);


  int sys_null (void);
  int sys_console (int subcommand, TCONSOLE *console, char *name);
  int sys_conwrite (char ch, int autoflush);
  int sys_conread (void);
  int sys_conflush (void);
  int fork (void);

#endif //__SERVICE_H__
