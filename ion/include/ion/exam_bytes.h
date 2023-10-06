#ifndef ION_EXAM_BYTES_H
#define ION_EXAM_BYTES_H

#include <stdint.h>

namespace Ion {
namespace ExamBytes {

typedef uint16_t Int;

Int read();
void write(Int examBytes);

}  // namespace ExamBytes
}  // namespace Ion

#endif
