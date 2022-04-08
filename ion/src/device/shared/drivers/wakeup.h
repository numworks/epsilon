#ifndef ION_DEVICE_WAKE_UP_H
#define ION_DEVICE_WAKE_UP_H

#include "regs/regs.h"

namespace Ion {
namespace Device {
namespace WakeUp {

/* All wakeup functions can be called together without overwriting the same
 * register. All together, they will set SYSCFG and EXTi registers as follow:
 *
 * GPIO Pin Number|EXTI_EMR|EXTI_FTSR|EXTI_RTSR|EXTICR1|EXTICR2|EXTICR3| Wake up
 * ---------------+--------+---------+---------+-------+-------+-------+-------------------------
 *        0       |   1    |    0    |    1    |   A   | ***** | ***** | Rising edge GPIO A pin 0
 *        1       |   1    |    1    |    0    |   C   | ***** | ***** | Falling edge GPIO C pin 1
 *        2       |   0    |    0    |    0    |   A   | ***** | ***** |
 *        3       |   0    |    0    |    0    |   A   | ***** | ***** |
 *        4       |   0    |    0    |    0    | ***** |   A   | ***** |
 *        5       |   0    |    0    |    0    | ***** |   A   | ***** |
 *        6       |   0    |    0    |    0    | ***** |   A   | ***** |
 *        7       |   0    |    0    |    0    | ***** |   A   | ***** |
 *        8       |   0    |    0    |    0    | ***** | ***** |   A   |
 *        9       |   1    |    1    |    1    | ***** | ***** |   A   | Falling/Rising edge GPIO A pin 9
 *       10       |   0    |    0    |    0    | ***** | ***** |   A   |
 *       11       |   0    |    0    |    0    | ***** | ***** |   A   |
 *       12       |   0    |    0    |    0    | ***** | ***** | ***** |
 *       13       |   0    |    0    |    0    | ***** | ***** | ***** |
 *       14       |   0    |    0    |    0    | ***** | ***** | ***** |
 *       15       |   0    |    0    |    0    | ***** | ***** | ***** |
 */

void onChargingEvent();
void onUSBPlugging();
void onOnOffKeyDown();

}
}
}

#endif
