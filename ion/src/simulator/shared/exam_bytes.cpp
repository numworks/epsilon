#include <ion/exam_bytes.h>

namespace Ion {
namespace ExamBytes {

Int s_examBytes = 0;

Int read() { return s_examBytes; }
void write(Int examBytes) { s_examBytes = examBytes; }

}  // namespace ExamBytes
}  // namespace Ion
