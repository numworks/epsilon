#ifndef ION_DEVICE_SD_CARD_H
#define ION_DEVICE_SD_CARD_H

extern "C" {
#include <stdint.h>
}

namespace Ion {
namespace SDCard {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA8 | SDIO D1           | Alternate Function 12 | SDIO_D1
 *  PB4 | SDIO D0           | Alternate Function 12 | SDIO_D0
 *  PB5 | SDIO D3           | Alternate Function 12 | SDIO_D3
 * PB15 | SDIO CLK          | Alternate Function 12 | SDIO_CK
 * PC10 | SDIO D2           | Alternate Function 12 | SDIO_D2
 *  PD2 | SDIO CMD          | Alternate Function 12 | SDIO_CMD
 */

void init();
void initGPIO();

void initCard();

void sendCommand(uint32_t cmd, uint32_t arg);

}
}
}

#endif
