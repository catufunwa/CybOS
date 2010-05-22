/******************************************************************************
 *
 *  File        : vfs.h
 *  Description : Virtual File System
 *
 *****************************************************************************/
#ifndef __VFS_H__
#define __VFS_H__

    #include "ktype.h"
    #include "device.h"

    // Defines for flags
    #define FS_FILE          0x01
    #define FS_DIRECTORY     0x02
    #define FS_CHARDEVICE    0x03   // Major and minor must be set
    #define FS_BLOCKDEVICE   0x04   // Major and minor must be set
    #define FS_PIPE          0x05
    #define FS_SYMLINK       0x06
    #define FS_MOUNTPOINT    0x08

    // Inode define
    typedef Uint32 inode_t;

    struct vfs_node; // Forward declaration

/*
    // All available file operations
    struct vfs_file_ops {
    };


    struct vfs_t {
      Uint32(*read)(struct vfs_node *, Uint32, Uint32, char *);
      Uint32(*write)(struct vfs_node *, Uint32, Uint32, char *);
      void (*open)(struct vfs_node *);
      void (*close)(struct vfs_node *);
      struct dirent * (*readdir)(struct vfs_node *, Uint32);
      struct fs_node * (*finddir)(struct vfs_node *, const char *);
    };
*/

    typedef struct {
      device_t  *device;       // Which device is used to acces this mount

      Uint32(*read)(struct vfs_node *, Uint32, Uint32, char *);
      Uint32(*write)(struct vfs_node *, Uint32, Uint32, char *);
      void (*open)(struct vfs_node *);
      void (*close)(struct vfs_node *);
      struct dirent * (*readdir)(struct vfs_node *, Uint32);
      struct vfs_node * (*finddir)(struct vfs_node *, const char *);
    } vfs_fileops_t;

    // Directory entry
    typedef struct dirent {
        char    name[128];    // Name of the directory
        inode_t inode_nr;     // Inode
    } vfs_dirent_t;

    // File entry
    typedef struct vfs_node {
        inode_t       inode_nr;        // Inode
        char          name[128];       // Filename
        Uint32        owner;           // Owner ID
        Uint32        length;          // File length
        Uint32        flags;           // Node type
        Uint8         majorNum;        // Major number (only for devices)
        Uint8         minorNum;        // Minor number (only for devices)

        vfs_fileops_t *fileops;        // Link to the file operations for this file
    } vfs_node_t;


    // Root of the filesystem
    extern vfs_node_t *vfs_root;

    // File system functions
    Uint32 vfs_read (vfs_node_t *node, Uint32 offset, Uint32 size, char *buffer);
    Uint32 vfs_write (vfs_node_t *node, Uint32 offset, Uint32 size, char *buffer);
    void vfs_create (vfs_node_t *node, const char *name);
    void vfs_open (vfs_node_t *node);
    void vfs_close (vfs_node_t *node);
    vfs_dirent_t *vfs_readdir (vfs_node_t *node, Uint32 index);
    vfs_node_t *vfs_finddir (vfs_node_t *node, const char *name);

    void vfs_init (void);


#endif // __VFS_H__
