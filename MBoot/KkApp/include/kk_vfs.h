#ifndef _VFS_H
#define _VFS_H

int kk_vfs_init(void);

int kk_vfs_mount(char *partition);

unsigned int  kk_vfsfile_getsize(char *filedir);

int kk_vfsfile_load(char *filedir,unsigned int  addr);

int kk_vfs_deinit(void);


#endif

