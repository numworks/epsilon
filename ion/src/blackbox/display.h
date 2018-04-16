#ifndef ION_BLACKBOX_DISPLAY_H
#define ION_BLACKBOX_DISPLAY_H

#include <kandinsky.h>

namespace Ion {
namespace Display {
namespace Blackbox {

const KDColor * frameBufferAddress();
void setFrameBufferActive(bool enabled);
void writeFrameBufferToFile(const char * filename);

}
}
}

#endif
