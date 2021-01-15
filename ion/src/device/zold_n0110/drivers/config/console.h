#ifndef ION_DEVICE_N0110_CONFIG_CONSOLE_H
#define ION_DEVICE_N0110_CONFIG_CONSOLE_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Console {
namespace Config {

using namespace Regs;

constexpr static USART Port = USART(6);
constexpr static GPIOPin RxPin = GPIOPin(GPIOC, 7);
constexpr static GPIOPin TxPin = GPIOPin(GPIOC, 6);
constexpr static GPIO::AFR::AlternateFunction AlternateFunction = GPIO::AFR::AlternateFunction::AF8;

/* The baud rate of the UART is set by the following equation:
 * BaudRate = f/USARTDIV, where f is the clock frequency and USARTDIV a divider.
 * In other words, USARTDIV = f/BaudRate. All frequencies in Hz.
 *
 * In our case, we configure the minicom to use a 115200 BaudRate and
 * f = fAPB2 = 96 MHz, so USARTDIV = 833.333 */
constexpr static int USARTDIVValue = 833;


}
}
}
}

#endif
