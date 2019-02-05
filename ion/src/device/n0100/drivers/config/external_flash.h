#ifndef ION_DEVICE_N0100_CONFIG_EXTERNAL_FLASH_H
#define ION_DEVICE_N0100_CONFIG_EXTERNAL_FLASH_H

#include <regs/regs.h>

/*  Pin | Role                 | Mode                  | Function
 * -----+----------------------+-----------------------+-----------------
 *  PB2 | QUADSPI CLK          | Alternate Function  9 | QUADSPI_CLK
 *  PB6 | QUADSPI BK1_NCS      | Alternate Function 10 | QUADSPI_BK1_NCS
 *  PC8 | QUADSPI BK1_IO2/WP   | Alternate Function  9 | QUADSPI_BK1_IO2
 *  PC9 | QUADSPI BK1_IO0/SO   | Alternate Function  9 | QUADSPI_BK1_IO0
 * PD12 | QUADSPI BK1_IO1/SI   | Alternate Function  9 | QUADSPI_BK1_IO1
 * PD13 | QUADSPI BK1_IO3/HOLD | Alternate Function  9 | QUADSPI_BK1_IO3
 */

namespace Ion {
namespace Device {
namespace ExternalFlash {
namespace Config {

constexpr static GPIOPin QSPIPins[] = {
  GPIOPin(GPIOB, 2), GPIOPin(GPIOB, 6), GPIOPin(GPIOC, 9), GPIOPin(GPIOD,12),
  GPIOPin(GPIOC, 8), GPIOPin(GPIOD,13)
};

}
}
}
}

#endif
