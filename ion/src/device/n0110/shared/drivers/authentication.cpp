#include <drivers/authentication.h>
#include <shared/drivers/config/board.h>
#include <drivers/flash.h>
#include <drivers/hash.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Authentication {

void decrypt(uint8_t * signature, uint8_t * decryptedSignature) {
  // TODO : Decrypt signature with public key
  memcpy(decryptedSignature, signature, Hash::Sha256DigestBytes);
}

bool isAuthenticated(void * pointer) {
  /* The standard memory layout is:
   * | code size |         code        |   signature   |
   * */
  if (reinterpret_cast<uint32_t>(pointer) == 0xFFFFFFFF) {
    // Special case for empty memory block
    return true;
  }
  uint32_t start = reinterpret_cast<uint32_t>(pointer);
  uint32_t size = 0;
  uint8_t * code = nullptr;
  if (!Flash::AddressIsInInternalFlash(start) && !Flash::AddressIsInExternalFlash(start)) {
    // Out of memory
    return false;
  }
  if (Flash::AddressIsInInternalFlash(start)) {
    // Internal flash code is unsigned and utterly fills the flash
    size = Board::Config::KernelInternalLength - Board::Config::SignatureSize;
    code = static_cast<uint8_t *>(pointer);
  }
  if (Flash::AddressIsInExternalFlash(start)) {
    size = *static_cast<uint32_t *>(pointer);
    if (!Flash::AddressIsInExternalFlash(start + size)) {
      // Overflow of external memory
      return false;
    }
    code = static_cast<uint8_t *>(pointer) + Board::Config::SizeSize;
  }
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
