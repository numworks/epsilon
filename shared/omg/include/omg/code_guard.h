#ifndef OMG_CODE_GUARD_H
#define OMG_CODE_GUARD_H

#include <omg/memory.h>

namespace OMG {

/* Place a CODE_GUARD macro around code that shouldn't be changed lightly. The
 * program will refuse to compile if the guarded code is changed without also
 * updating the CRC, making it more obvious than a simple comment.
 *
 * Note: Purposefully use #__VA_ARGS__ instead of a classic STRINGIFY macro, to
 * avoid expanding macros in the computation of the CRC. Indeed, some macros
 * definitions depend on the compilation target or flags (e.g. assert) and would
 * lead to inconsistent CRC across builds. */
#define CODE_GUARD(id, reference_crc, ...)                                 \
  static_assert(OMG::Memory::crc32String(#__VA_ARGS__) == reference_crc,   \
                #id ": modifying this code might break an external API!"); \
  __VA_ARGS__

}  // namespace OMG

#endif
