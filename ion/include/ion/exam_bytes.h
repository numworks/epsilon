#ifndef ION_EXAM_BYTES_H
#define ION_EXAM_BYTES_H

#include <stdint.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Ion {
namespace ExamBytes {

using Int =
#if __EMSCRIPTEN__
    emscripten_align1_short;
#else
    uint16_t;
#endif

constexpr ExamBytes::Int k_defaultValue = 0;

Int read();
void write(Int examBytes);

}  // namespace ExamBytes
}  // namespace Ion

#endif
