/*
 */

#ifndef _NTFS_UNISTR_H
#define _NTFS_UNISTR_H

#include "types.h"
#include "layout.h"
#include <mstar/mpatch_macro.h>

#if (MP_NTFS_VOLUME_LABEL==1)
extern int ntfs_ucstombs(const ntfschar *ins, const int ins_len, char **outs,
		int outs_len);
#endif

#endif /* defined _NTFS_UNISTR_H */

