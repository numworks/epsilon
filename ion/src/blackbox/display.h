#ifndef ION_BLACKBOX_DISPLAY_H
#define ION_BLACKBOX_DISPLAY_H

namespace Ion {
namespace Display {
namespace Blackbox {

void setFrameBufferActive(bool enabled);
void writeFrameBufferToFile(const char * filename);

}
}
}

#endif
