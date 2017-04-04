#ifndef ION_DEVICE_CONSOLE_H
#define ION_DEVICE_CONSOLE_H

#include <ion/console.h>
#include "regs/regs.h"

namespace Ion {
namespace Console {
namespace Device {

/*  Pin | Role              | Mode
 * -----+-------------------+--------------------
 * PC11 | UART3 RX          | Alternate Function
 *  PD8 | UART3 TX          | Alternate Function
 */

void init();
void shutdown();
bool peerConnected();

constexpr USART UARTPort = USART(3);
constexpr static GPIOPin RxPin = GPIOPin(GPIOC, 11);
constexpr static GPIOPin TxPin = GPIOPin(GPIOD, 8);
constexpr static GPIOPin Pins[] = { RxPin, TxPin };

}
}
}

#endif
