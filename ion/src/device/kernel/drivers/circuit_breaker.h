#ifndef ION_DEVICE_KERNEL_CHECKPOINT_H
#define ION_DEVICE_KERNEL_CHECKPOINT_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Checkpoint {

bool hasCheckpoint();
/* return true if error !*/
void setCheckpoint(uint8_t * frameAddress, uint32_t excReturn);
/* ATTENTION a cause de ASPEN, faire une instruction float pour être sure de loader les s0-s31 avant de les enregistrer*/
void unsetCheckpoint();
void loadCheckpoint(uint8_t * frameAddress);

}
}
}

#endif
