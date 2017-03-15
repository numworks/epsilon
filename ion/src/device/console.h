#ifndef ION_DEVICE_CONSOLE_H
#define ION_DEVICE_CONSOLE_H

#include <ion/console.h>
#include "regs/regs.h"

namespace Ion {
namespace Console {
namespace Device {

/*  Pin | Role              | Mode
 * -----+-------------------+--------------------
 * PC11 | UART1 RX          | Alternate Function
 *  PD8 | UART1 TX          | Alternate Function
 */

void init();
void shutdown();

//constexpr USART UARTPort = USART(3);
//constexpr static GPIOPin Pins[] = { GPIOPin(GPIOC, 11), GPIOPin(GPIOD, 8) };

constexpr USART UARTPort = USART(1);
constexpr static GPIOPin Pins[] = { GPIOPin(GPIOB, 3), GPIOPin(GPIOB, 6) };

void sendChar(char c);
char recvChar();

}
}
}

#endif
