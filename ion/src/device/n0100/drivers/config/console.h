#ifndef ION_DEVICE_N0100_CONFIG_CONSOLE_H
#define ION_DEVICE_N0100_CONFIG_CONSOLE_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Console {
namespace Config {

using namespace Regs;

constexpr static USART Port = USART(3);
constexpr static GPIOPin RxPin = GPIOPin(GPIOC, 11);
constexpr static GPIOPin TxPin = GPIOPin(GPIOD, 8);
constexpr static GPIO::AFR::AlternateFunction AlternateFunction = GPIO::AFR::AlternateFunction::AF7;

/* The baud rate of the UART is set by the following equation:
 * BaudRate = f/USARTDIV, where f is the clock frequency and USARTDIV a divider.
 * In other words, USARTDIV = f/BaudRate. All frequencies in Hz.
 *
 * In our case, we configure the minicom to use a 115200 BaudRate and
 * f = fAPB1 = 48 MHz, so USARTDIV = 416.666 */
constexpr static int USARTDIVValue = 417;


}
}
}
}

#endif
