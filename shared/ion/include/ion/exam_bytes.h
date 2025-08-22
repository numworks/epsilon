#pragma once

#include <omg/unaligned.h>
#include <stdint.h>

namespace Ion {
namespace ExamBytes {

typedef OMG::unaligned_uint16_t Int;

constexpr ExamBytes::Int k_defaultValue = 0;

Int read();
void write(Int examBytes);

}  // namespace ExamBytes
}  // namespace Ion
