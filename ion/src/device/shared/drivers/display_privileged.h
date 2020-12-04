#ifndef ION_DEVICE_SHARED_DISPLAY_PRIVILEGED_H
#define ION_DEVICE_SHARED_DISPLAY_PRIVILEGED_H

#include <kandinsky/rect.h>
#include <kandinsky/color.h>
extern "C" {
#include <stddef.h>
}

namespace Ion {
namespace Device {
namespace Display {

// void pushRectSVC(); //const KDColor * pixels, size_t numberOfPixels);
void pushRectUniformSVC(); //KDColor color, size_t numberOfPixels);

}
}
}

#endif