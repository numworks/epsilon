#ifndef OMG_CODE_GUARD_H
#define OMG_CODE_GUARD_H

#include <ion/crc.h>

namespace OMG {

#define CODE_GUARD_STRINGIFY(...) #__VA_ARGS__

/* Place a CODE_GUARD macro around code that shouldn't be changed lightly. The
 * program will refuse to compile if the guarded code is changed without also
 * updating the CRC, making it more obvious than a simple comment. */
#define CODE_GUARD(id, reference_crc, ...)                                  \
  static_assert(                                                            \
      Ion::crc32String(CODE_GUARD_STRINGIFY(__VA_ARGS__)) == reference_crc, \
      CODE_GUARD_STRINGIFY(                                                 \
          id) ": modifying this code might break an external API!");        \
  __VA_ARGS__

}  // namespace OMG

#endif
