#include <drivers/authentication.h>
#include <drivers/flash.h>
#include <drivers/hash.h>
#include <kernel/drivers/config/board.h>
#include <assert.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Authentication {

enum class Status {
  Unknown,
  Official,
  Unofficial
};

bool isAuthenticated(void * pointer) {
  static Status sStatus = Status::Unknown;
  if (sStatus == Status::Unknown) {
    /* The standard memory layout is:
     * | code size |         code        |   signature   |
     * */
    uint32_t start = reinterpret_cast<uint32_t>(pointer);
    if (!Flash::AddressIsInExternalFlash(start)) {
      // Out of memory
      sStatus = Status::Unofficial;
      return false;
    }
    uint32_t size = *static_cast<uint32_t *>(pointer);
    if (!Flash::AddressIsInExternalFlash(start + size)) {
      // Overflow of external memory
      sStatus = Status::Unofficial;
      return false;
    }
    uint8_t * code = static_cast<uint8_t *>(pointer) + Board::Config::SizeSize;
    assert(code && size > 0);
    uint8_t digest[Ion::Device::Hash::Sha256DigestBytes];
    Hash::sha256(code, size, digest);
    uint8_t * signature = static_cast<uint8_t *>(code) + size;
    if (verify(signature, digest, Hash::Sha256DigestBytes)) {
      sStatus = Status::Official;
    } else {
      sStatus = Status::Unofficial;
    }
  }
  return sStatus == Status::Official;
}

}
}
}
