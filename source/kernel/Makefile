.SILENT :

# Standard compiler programs
CC       = gcc
LD       = ld
AS       = nasm
OBJCOPY  = objcopy
RM       = rm

# Linker flags
LD_ENTRY  = -e kernel_entry
LD_TEXT   = --section-start .text=0xC0010000 -Map kernel.map

#LIBS =  ../stdlib/stdlib.a
LIBS =


# Main target
TARGET = ../../binary/kernel/cybos32.bin

DEBUG_FLAGS = -ggdb

# Compiler and user flags
CC_FLAGS = -m32 -nostdlib -nostdinc -fno-pic -fno-builtin -fno-exceptions -fomit-frame-pointer -Wall $(DEBUG_FLAGS)

USR_FLAGS = -D__KERNEL__ -D__DEBUG__

#INCLUDES = -I. -I.. -I../stdinc -Iinclude
INCLUDES = -I. -I.. -Iinclude

CFLAGS = $(CC_FLAGS) $(INCLUDES) $(USR_FLAGS)


# Objects
OBJS =  main.o \
        kernel.o \
        console.o \
        conio.o \
        kmem.o \
        heap.o \
        page.o \
        paging.o \
        gdt.o \
        idt.o \
        isr.o \
        schedule.o \
        task.o \
        tss.o \
        timer.o \
        keyboard.o \
        service.o \
        command.o \
        queue.o \
        io.o \
        pit.o \
        pic.o \
        mutex.o \
        mutexasm.o \
        klib.o \
        pci.o \
        dma.o \
        exec.o \
        device.o \
        drivers/floppy/floppy.o \
        drivers/ide/ide.o \
        drivers/ide/ata.o \
        drivers/ide/atapi.o \
        drivers/ide/partitions.o \
        vfs.o \
        vfs/fat12.o \
	vfs/ext2.o \
        vfs/cybfs.o \
        vfs/devfs.o \
        ff/elf.o


# Default target
all: kernel

# Cleans up the kernel objects
clean:
	echo [RM]
	$(RM) *.o || true
	$(RM) $(TARGET) || true
	$(RM) $(TARGET).sym || true
	$(RM) $(TARGET).elf || true
	$(RM) *.map || true


kernel: $(OBJS)
	echo [LD] $(TARGET)
	$(LD) $(LD_TEXT) $(LD_ENTRY) -o $(TARGET).elf -g $(OBJS) $(LIBS) $(DEBUG_FLAGS)
	$(OBJCOPY) --only-keep-debug $(TARGET).elf $(TARGET).sym
#	$(LD) $(LD_TEXT) $(LD_ENTRY) -o $(TARGET).elf -g $(OBJS) $(LIBS) $(DEBUG_FLAGS)
	$(LD) $(LD_TEXT) $(LD_ENTRY) -s -o $(TARGET) --oformat binary $(OBJS) $(LIBS)

# Implicit Rules
.c.o:
	echo [CC] $<
	$(CC) $(CFLAGS) -c -o $@ $<
.S.o:
	echo [AS] $<
	$(AS) -f elf -o $@ $<
