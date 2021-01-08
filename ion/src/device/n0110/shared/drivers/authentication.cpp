#include <drivers/authentication.h>
#include <shared/drivers/config/board.h>
#include <drivers/flash.h>
#include <drivers/hash.h>
#include <assert.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Authentication {

void decrypt(uint8_t * signature, uint8_t * decryptedSignature) {
  // TODO : Decrypt signature with public key
  memcpy(decryptedSignature, signature, Hash::Sha256DigestBytes);
}

}
}
}
