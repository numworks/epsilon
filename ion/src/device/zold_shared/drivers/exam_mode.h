#include <stdint.h>
#include <stddef.h>

namespace Ion {
namespace ExamMode {

constexpr size_t numberOfBitsInByte = 8;
size_t numberOfBitsAfterLeadingZeroes(int i);
uint8_t * SignificantExamModeAddress();

}
}
