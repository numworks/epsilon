#ifndef ION_DEVICE_SHARED_BASE64_H
#define ION_DEVICE_SHARED_BASE64_H

namespace Ion {
namespace Device {
namespace Base64 {

void encode(const unsigned char* input, unsigned int inputLength, char* output);

}
}  // namespace Device
}  // namespace Ion

#endif
