#ifndef ION_DEVICE_N0100_CONFIG_CONSOLE_H
#define ION_DEVICE_N0100_CONFIG_CONSOLE_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Console {
namespace Config {

constexpr static USART Port = USART(3);
constexpr static GPIOPin RxPin = GPIOPin(GPIOC, 11);
constexpr static GPIOPin TxPin = GPIOPin(GPIOD, 8);
constexpr static GPIO::AFR::AlternateFunction AlternateFunction = GPIO::AFR::AlternateFunction::AF8;

}
}
}
}

#endif
