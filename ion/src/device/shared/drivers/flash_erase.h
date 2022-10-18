#ifndef ION_DEVICE_SHARED_FLASH_ERASE_H
#define ION_DEVICE_SHARED_FLASH_ERASE_H

namespace Ion {
namespace Device {
namespace Flash {

void MassErase();
bool EraseSector(int i);

}
}
}

#endif