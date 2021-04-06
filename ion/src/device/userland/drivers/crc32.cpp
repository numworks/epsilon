#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

uint32_t SVC_ATTRIBUTES crc32Word(const uint32_t * data, size_t length) {
  SVC(SVC_CRC32_WORD);
}

uint32_t SVC_ATTRIBUTES crc32Byte(const uint8_t * data, size_t length) {
  SVC(SVC_CRC32_BYTE);
}

}
