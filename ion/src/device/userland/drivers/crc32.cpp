#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

void SVC_ATTRIBUTES crc32WordSVC(const uint32_t * data, size_t * length, uint32_t * res) {
  SVC(SVC_CRC32_WORD);
}

uint32_t crc32Word(const uint32_t * data, size_t length) {
  uint32_t res;
  crc32WordSVC(data, &length, &res);
  return res;
}

void SVC_ATTRIBUTES crc32ByteSVC(const uint8_t * data, size_t * length, uint32_t * res) {
  SVC(SVC_CRC32_BYTE);
}

uint32_t crc32Byte(const uint8_t * data, size_t length) {
  uint32_t res;
  crc32ByteSVC(data, &length, &res);
  return res;
}

}
