#include <drivers/authentication.h>
#include <drivers/config/board.h>
#include <drivers/config/external_flash.h>
#include <drivers/hash.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Authentication {

void decrypt(uint8_t * signature, uint8_t * decryptedSignature) {
  // TODO : Decrypt signature with public key
  memcpy(decryptedSignature, signature, Hash::Sha256DigestBytes);
}

bool authenticatesContiguousCode(uint8_t * code, uint32_t size) {
  uint8_t digest[Ion::Device::Hash::Sha256DigestBytes];
  uint8_t decryptedSignature[Hash::Sha256DigestBytes];
  Hash::sha256(code, size, digest);
  uint8_t * signature = static_cast<uint8_t *>(code) + size;
  decrypt(signature, decryptedSignature);
  return memcmp(digest, decryptedSignature, Hash::Sha256DigestBytes) == 0;
}

bool isAuthenticated(void * pointer) {
  /* The standard memory layout is:
   * | code size |         code        |   signature   |
   *
   * Warning: the code can overlap both flashes and in this case both parts are
   * signed and the memory layout is the following:
   *
   * - Internal flash:
   * | code size |         code        |   signature   |
   * - External flash:
   * |         code        | signature |
   *
   * */

  bool authentication = true;

  uint32_t size = *static_cast<uint32_t *>(pointer);
  uint8_t * code = static_cast<uint8_t *>(pointer) + Board::Config::SizeSize;
  uint32_t codeAddress = reinterpret_cast<uint32_t>(code);
  if (Board::Config::VirtualInternalStartAddress <= codeAddress && codeAddress <= Board::Config::VirtualInternalEndAddress && size >= (Board::Config::VirtualInternalEndAddress - codeAddress)) {
    // The code overlaps on both flash
    uint32_t internalSize = Board::Config::VirtualInternalEndAddress - codeAddress - Board::Config::SignatureSize;
    authentication = authenticatesContiguousCode(code, internalSize);
    size -= internalSize;
    code = reinterpret_cast<uint8_t *>(ExternalFlash::Config::StartAddress);
  }
  authentication = authentication && authenticatesContiguousCode(code, size);
  return authentication;
}

}
}
}
