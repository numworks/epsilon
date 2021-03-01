#include <drivers/authentication.h>
#include <shared/drivers/config/board.h>
#include <drivers/flash.h>
#include <assert.h>
#include <string.h>
#include <ion/src/external/libsodium/include/sodium/crypto_sign.h>

namespace Ion {
namespace Device {
namespace Authentication {

/* TODO: remove me! + update public key
 * Private key: 0x89a991a29f5e310bc5b6cfb46cd0a66f4fb1b63fb40cf2e46fb1fa42a201fed162339e856ef221f2a44eea1fe3ffb5092c07267a7d990a5b0eba6ee19f25dbe7
 * Public key: 0x62339e856ef221f2a44eea1fe3ffb5092c07267a7d990a5b0eba6ee19f25dbe7
 */

bool verify(uint8_t * signature, uint8_t * code, uint32_t size) {
  unsigned char publicKey[crypto_sign_PUBLICKEYBYTES] = {0x62, 0x33, 0x9e, 0x85, 0x6e, 0xf2, 0x21, 0xf2, 0xa4, 0x4e, 0xea, 0x1f, 0xe3, 0xff, 0xb5, 0x09, 0x2c, 0x07, 0x26, 0x7a, 0x7d, 0x99, 0x0a, 0x5b, 0x0e, 0xba, 0x6e, 0xe1, 0x9f, 0x25, 0xdb, 0xe7};

  crypto_sign_state state;
  crypto_sign_init(&state);
  static constexpr int k_codeChunckSize = 32;
  uint8_t * endCode = code + size;
  while (code < endCode) {
    crypto_sign_update(&state, code, code + k_codeChunckSize >= endCode ? endCode - code : k_codeChunckSize);
    code += k_codeChunckSize;
  }
  return crypto_sign_final_verify(&state, signature, publicKey) == 0;
}

}
}
}
