/*
  Guidelines for creating a custom version of MORECORE:

  * For best performance, MORECORE should allocate in multiples of pagesize.
  * MORECORE may allocate more memory than requested. (Or even less,
      but this will usually result in a malloc failure.)
  * MORECORE must not allocate memory when given argument zero, but
      instead return one past the end address of memory from previous
      nonzero call.
  * For best performance, consecutive calls to MORECORE with positive
      arguments should return increasing addresses, indicating that
      space has been contiguously extended.
  * Even though consecutive calls to MORECORE need not return contiguous
      addresses, it must be OK for malloc'ed chunks to span multiple
      regions in those cases where they do happen to be contiguous.
  * MORECORE need not handle negative arguments -- it may instead
      just return MFAIL when given negative arguments.
      Negative arguments are always multiples of pagesize. MORECORE
      must not misinterpret negative args as large positive unsigned
      args. You can suppress all such calls from even occurring by defining
      MORECORE_CANNOT_TRIM,

  As an example alternative MORECORE, here is a custom allocator
  kindly contributed for pre-OSX macOS.  It uses virtually but not
  necessarily physically contiguous non-paged memory (locked in,
  present and won't get swapped out).  You can use it by uncommenting
  this section, adding some #includes, and setting up the appropriate
  defines above:

      #define MORECORE osMoreCore
*/
#include <types.h>  /* For size_t */
#include <asm/mips-boards/prom.h>
#include <kernel.h>

#define MAX_SIZE_T  (~(size_t)0)
#define MFAIL       ((void*)(MAX_SIZE_T))

#undef PRINT_G3D_INFO
#define PRINT_G3D_INFO(fmt, args...)  printk(KERN_INFO "[G3D]" fmt, ## args)

static void *g3d0sbrk_top = NULL;
static unsigned int g3d0addrend;

void *g3d0mysbrk(int size)
{
    void *ptr = 0;

    if (g3d0sbrk_top == NULL)
    {
        unsigned int addr, len;
        get_boot_mem_info(G3D_MEM0, &addr, &len);
        g3d0sbrk_top = (void*)addr;
        g3d0addrend = (unsigned int)((unsigned int)g3d0sbrk_top + len);

        PRINT_G3D_INFO("sbrk[0]: 0x%x--0x%x\n", (unsigned int)g3d0sbrk_top, (unsigned int)g3d0addrend);
    }

    if (size > 0)
    {
        if (((unsigned int)g3d0sbrk_top + (unsigned int)size) > g3d0addrend)
        {
            PRINT_G3D_INFO("sbrk[0] oversize: 0x%08X\n", ((unsigned int)g3d0sbrk_top + (unsigned int)size));
            return (void*)MFAIL;
        }

        ptr = g3d0sbrk_top;
        g3d0sbrk_top = (char*)ptr + size;
        return ptr;
    }
    else if (size < 0)
    {
        PRINT_G3D_INFO("sbrk[0] shrink: not supported\n");
        return (void*)MFAIL;
    }
    else
    {
        return g3d0sbrk_top;
    }
}

static void *g3d1sbrk_top = NULL;
static unsigned int g3d1addrend;

void *g3d1mysbrk(int size)
{
    void *ptr = 0;

    if (g3d1sbrk_top == NULL)
    {
        unsigned int addr, len;
        get_boot_mem_info(G3D_MEM1, &addr, &len);
        g3d1sbrk_top = (void*)addr;
        g3d1addrend = (unsigned int)((unsigned int)g3d1sbrk_top + len);
        PRINT_G3D_INFO("sbrk[1]: 0x%x--0x%x\n", (unsigned int)g3d1sbrk_top, (unsigned int)g3d1addrend);
    }

    if (size > 0)
    {
        if (((unsigned int)g3d1sbrk_top + (unsigned int)size) > g3d1addrend)
        {
            PRINT_G3D_INFO("sbrk[1] oversize: 0x%08X\n", ((unsigned int)g3d1sbrk_top + (unsigned int)size));
            return (void*)MFAIL;
        }

        ptr = g3d1sbrk_top;
        g3d1sbrk_top = (char*)ptr + size;
        return ptr;
    }
    else if (size < 0)
    {
        PRINT_G3D_INFO("sbrk[1] shrink: not supported\n");
        return (void *) MFAIL;
    }
    else
    {
        return g3d1sbrk_top;
    }
}

