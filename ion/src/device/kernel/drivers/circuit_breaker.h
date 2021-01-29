#ifndef ION_DEVICE_KERNEL_CHECKPOINT_H
#define ION_DEVICE_KERNEL_CHECKPOINT_H

namespace Ion {
namespace Device {
namespace Checkpoint {

bool hasCheckpoint();
/* return true if error !*/
bool setCheckpoint();
/* ATTENTION a cause de ASPEN, faire une instruction float pour être sure de loader les s0-s31 avant de les enregistrer*/
void unsetCheckpoint();
void loadCheckpoint();

}
}
}

#endif
