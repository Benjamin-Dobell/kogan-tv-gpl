#ifndef _MDRV_SOFTWARE_IR_IO_H_
#define _MDRV_SOFTWARE_IR_IO_H_

//#include "mdrv_types.h"

#define SOFTWARE_IR_IOC_MAGIC                'u'

#define MDRV_SOFTWARE_IR_SEND_KEY             _IOW(SOFTWARE_IR_IOC_MAGIC, 0, int)
#define MDRV_SOFTWARE_IR_GET_KEY              _IOW(SOFTWARE_IR_IOC_MAGIC, 1, int)
#define MDRV_SOFTWARE_IR_TAKE_OVER            _IOW(SOFTWARE_IR_IOC_MAGIC, 2, int)
#define MDRV_SOFTWARE_IR_DROP_OUT             _IOW(SOFTWARE_IR_IOC_MAGIC, 3, int)
#define MDRV_SOFTWARE_IR_PLAY_SINGLE_KEY      _IOW(SOFTWARE_IR_IOC_MAGIC, 4, int)



#define SOFTWARE_IR_IOC_MAXNR                5




#endif // _MDRV_SOFTWARE_IR_IO_H_


