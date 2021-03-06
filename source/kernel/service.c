/******************************************************************************
 *
 *  File        : service.c
 *  Description : Service Routine like DOS 0x21 interrupt
 *
 *****************************************************************************/
#include "kernel.h"
#include "service.h"
#include "conio.h"
#include "console.h"
#include "schedule.h"
#include "keyboard.h"
#include "exec.h"


/* These macro creates an <func>() function that does a syscall (INT 42) call with the correct
 * parameters. Since syscalls accept a variable number of parameters (some 0, some 1, some even
 * more), we have create_syscall_entryX(), where X stands for the number of parameters. */
CREATE_SYSCALL_ENTRY1(exit,    SYS_EXIT, char)
CREATE_SYSCALL_ENTRY0(getppid, SYS_GETPPID)
CREATE_SYSCALL_ENTRY0(getpid,  SYS_GETPID)
CREATE_SYSCALL_ENTRY0(fork,    SYS_FORK)
CREATE_SYSCALL_ENTRY0(idle,    SYS_IDLE)
CREATE_SYSCALL_ENTRY0(signal,  SYS_SIGNAL)
CREATE_SYSCALL_ENTRY1(sleep,   SYS_SLEEP, int)
CREATE_SYSCALL_ENTRY3(execve,  SYS_EXECVE, char *, char **, char **)



  /***
   *
   * @TODO Could be handled by a lowlevel lookup-table, but this is probably a bit more readable
   *
   * Calling paramters:
   *   param 1 : EBX
   *   param 2 : ECX
   *   param 3 : EDX
   *   param 4 : ESI
   *   param 5 : EDI
   *   param 6 : time for using a structure instead of so many parameters...
   *
   *  This parameter list is defined in the create_syscall_entryX macro's (service.h)
   *
   */
  int service_interrupt (regs_t *r) {
    int retval = 0;
    int service = (r->eax & 0x0000FFFF);

    switch (service) {
      default        :
      case  SYS_NULL :
                      retval = sys_null ();
                      break;
      case  SYS_CONSOLE :
                      retval = sys_console (r->ebx, (console_t *)r->ecx, (char *)r->edx);
                      break;
      case  SYS_CONWRITE :
                      retval = sys_conwrite ((char)r->ebx, r->ecx);
                      break;
      case  SYS_CONFLUSH :
                      retval = sys_conflush ();
                      break;
      case  SYS_FORK :
                      retval = sys_fork (r);
                      break;
      case  SYS_GETPID :
                      retval = sys_getpid ();
                      break;
      case  SYS_GETPPID :
                      retval = sys_getppid ();
                      break;
      case  SYS_SLEEP :
                      retval = sys_sleep (r->ebx);
                      break;
      case  SYS_IDLE :
                      retval = sys_idle ();
                      break;
      case  SYS_EXIT :
                      retval = sys_exit (r->ebx);
                      break;
      case  SYS_EXECVE :
                      retval = sys_execve (r, (char *)r->ebx, (char **)r->ecx, (char **)r->edx);
                      break;
    }
    return retval;
  }


  // ========================================================
  int sys_null (void) {
    return 0;
  }

  // ========================================================
  int sys_conwrite (char ch, int autoflush) {
    con_putch (_current_task->console, ch);
    if (autoflush) sys_conflush ();
    return 0;
  }

  // ========================================================
  int sys_conread (void) {
    return keyboard_poll ();
  }

  // ========================================================
  int sys_conflush (void) {
    con_flush (_current_task->console);
    return 0;
  }
