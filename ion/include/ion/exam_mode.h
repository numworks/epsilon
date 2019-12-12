#ifndef ION_EXAM_MODE_H
#define ION_EXAM_MODE_H

extern "C" {
#include <stdint.h>
}

namespace Ion {
namespace ExamMode {

uint8_t FetchExamMode();
void IncrementExamMode(uint8_t delta);

}
}

#endif
