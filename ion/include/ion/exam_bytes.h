#ifndef ION_EXAM_BYTES_H
#define ION_EXAM_BYTES_H

#include <stdint.h>

namespace Ion {
namespace ExamBytes {

/* Warning : All persisting bytes are expected to be initialized to 0 in
 * shared_kernel_flash.ld which assumes ExamBytes::Int is 16 bits long */
typedef uint16_t Int;

Int read();
void write(Int examBytes);

}  // namespace ExamBytes
}  // namespace Ion

#endif
