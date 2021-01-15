#include <drivers/authentication.h>
#include <drivers/flash.h>
#include <drivers/hash.h>
#include <kernel/drivers/config/board.h>
#include <assert.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Authentication {

bool isAuthenticated(void * pointer) {
  /* The standard memory layout is:
   * | code size |         code        |   signature   |
   * */
  uint32_t start = reinterpret_cast<uint32_t>(pointer);
  if (!Flash::AddressIsInExternalFlash(start)) {
    // Out of memory
    return false;
  }
  uint32_t size = *static_cast<uint32_t *>(pointer);
  if (!Flash::AddressIsInExternalFlash(start + size)) {
    // Overflow of external memory
    return false;
  }
  uint8_t * code = static_cast<uint8_t *>(pointer) + Board::Config::SizeSize;
  assert(code && size > 0);
  uint8_t digest[Ion::Device::Hash::Sha256DigestBytes];
  uint8_t decryptedSignature[Hash::Sha256DigestBytes];
  Hash::sha256(code, size, digest);
  uint8_t * signature = static_cast<uint8_t *>(code) + size;
  decrypt(signature, decryptedSignature);
  return memcmp(digest, decryptedSignature, Hash::Sha256DigestBytes) == 0;
}

}
}
}
