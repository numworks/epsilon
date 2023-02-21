#include <drivers/svcall.h>
#include <ion.h>

namespace Ion {

uint32_t SVC_ATTRIBUTES crc32Word(const uint32_t* data, size_t length){
    SVC_RETURNING_R0(SVC_CRC32_WORD, uint32_t)}

uint32_t SVC_ATTRIBUTES crc32Byte(const uint8_t* data, size_t length) {
  SVC_RETURNING_R0(SVC_CRC32_BYTE, uint32_t)
}

}  // namespace Ion
