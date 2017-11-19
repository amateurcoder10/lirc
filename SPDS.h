
#define DRIVER_NAME "SPDS"

//---------------------------------------------------------------------------------//
/*				pulse space
 * header       9061  4453
   one           591  1658
   zero          591   539
   ptrail        597
   gap          108004
 *
 */


#define HEADER_PULSE 9055
#define HEADER_SPACE 4452

#define ONE_PULSE 588
#define ONE_SPACE 1660

#define ZERO_PULSE 588
#define ZERO_SPACE 542

#define TRAIL_PULSE 598

#define KEY_POWER 0x212FC23D
#define NUMBITS 32

#define GPIO_PIN 22

#ifndef MAX_UDELAY_MS
#define MAX_UDELAY_US 1000
#else
#define MAX_UDELAY_US (MAX_UDELAY_MS*1000)
#endif
//--------------------------------------------------------------------------------//

#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/timekeeping.h>

