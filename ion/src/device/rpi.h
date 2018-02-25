#ifndef ION_DEVICE_RPI_H
#define ION_DEVICE_RPI_H

#ifdef __cplusplus
#include <ion/rpi.h>

extern "C" void rpi_isr();

namespace Ion {
namespace Rpi {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PB9 | Power Enable      | Output                |
 *  PA5 | SPI Clock         | Input                 | AF5
 *  PA6 | SPI MISO          | Output                | AF5
 *  PA7 | SPI MOSI          | Input                 | AF5
 */

void init();
void shutdown();

constexpr GPIO ChipSelectGPIO = GPIOA;
constexpr uint8_t ChipSelectPin = 6;

}
}
}

constexpr static GPIOPin PowerPin = GPIOPin(GPIOB, 9);
constexpr static DMA DMAEngine = DMA2;
constexpr static int DMAStream = 2;

#else
void rpi_isr();
#endif

#endif
