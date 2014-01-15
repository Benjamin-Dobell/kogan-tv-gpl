
#ifndef _MDRV_G3D_IO_H_
#define _MDRV_G3D_IO_H_

/* Use 'G' as magic number */
#define G3D_IOC_MAGIC             'g'

#define G3D_IOC_INIT               _IO(G3D_IOC_MAGIC,   0x80)
#define G3D_IOC_CMDQ_RESET         _IO(G3D_IOC_MAGIC,   0x81)
#define G3D_IOC_CMDQ_SEND          _IOWR(G3D_IOC_MAGIC, 0x82, G3D_CMDQ_BUFFER)
#define G3D_IOC_GET_CMDQ           _IOR(G3D_IOC_MAGIC,  0x83, G3D_MALLOC_INFO)
#define G3D_IOC_IS_IDLE            _IOR(G3D_IOC_MAGIC,  0x84, int)
#define G3D_IOC_GET_SWSTAMP        _IOR(G3D_IOC_MAGIC,  0x85, int)
#define G3D_IOC_GET_HWSTAMP        _IOR(G3D_IOC_MAGIC,  0x86, int)

//for png/gif
#define G3D_IOC_LOCK               _IO(G3D_IOC_MAGIC,  0x87)
#define G3D_IOC_UNLOCK             _IO(G3D_IOC_MAGIC,  0x88)
#define G3D_IOC_PNGCMDQ_SEND       _IOWR(G3D_IOC_MAGIC, 0x89, G3D_CMDQ_BUFFER)

#define G3D_IOC_MALLOC             _IOWR(G3D_IOC_MAGIC, 0x8A, G3D_MALLOC_INFO)
#define G3D_IOC_MFREE              _IOW(G3D_IOC_MAGIC,  0x8B, G3D_MALLOC_INFO)
#define G3D_IOC_GETMMAP            _IOR(G3D_IOC_MAGIC,  0x8C, G3D_MMAP_BUFFER)
#define G3D_IOC_GETFILPID          _IOR(G3D_IOC_MAGIC,  0x8D, int)
#endif


