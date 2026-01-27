#include "runner.h"

#include <ion.h>
#include <poincare/print.h>

#include "cases.h"

static bool clearConsole() { return Ion::Console::clear(); }

namespace Quiz {

Runner Runner::s_instance;

void Runner::innerRun() {
  constexpr int k_bufferSize = 64;
  char buffer[k_bufferSize];
  TestsMetadata metadata = computeTestsMetadata();

  int t0 = Ion::Timing::millis();
  int caseIndex = 0;
  for (int i = metadata.startIndex; i < metadata.endIndex; i++) {
    if (m_testFilter &&
        strstr(Cases::s_names[i], m_testFilter) != Cases::s_names[i]) {
      continue;
    }
    caseIndex++;
    if (clearConsole()) {
      // Avoid cluttering the display if it can't be cleared
      Poincare::Print::CustomPrintf(buffer, k_bufferSize, "TEST: %i/%i",
                                    caseIndex, metadata.totalCases);
      quiz_print(buffer);
    }
    quiz_print(Cases::s_names[i]);
    Env::Flush();
    Cases::s_bodies[i]();
  }
  Env::Flush();
  clearConsole();
  if (m_numberOfChunks != 1) {
    return;
  }

  // Display test results
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "ALL %i TESTS FINISHED",
                                metadata.totalCases);
  quiz_print(buffer);
  // Display test duration
  int time = Ion::Timing::millis() - t0;
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "DURATION: %i ms", time);
  quiz_print(buffer);
}

Runner::TestsMetadata Runner::computeTestsMetadata() {
  TestsMetadata metadata;
  metadata.startIndex = 0;
  metadata.endIndex = Cases::s_number;
  metadata.totalCases = 0;

  // Apply chunking
  if (m_numberOfChunks > 1) {
    int casesPerChunk =
        (Cases::s_number + m_numberOfChunks - 1) / m_numberOfChunks;
    metadata.startIndex = m_chunkId * casesPerChunk;
    metadata.endIndex =
        std::min(metadata.startIndex + casesPerChunk, Cases::s_number);
  }

  // Count matching tests
  if (m_fromFilter) {
    int fromIndex = metadata.endIndex;
    for (int i = metadata.startIndex; i < metadata.endIndex; i++) {
      if (strstr(Cases::s_names[i], m_fromFilter) == Cases::s_names[i]) {
        fromIndex = i;
        break;
      }
    }
    metadata.startIndex = fromIndex;
  }
  for (int i = metadata.startIndex; i < metadata.endIndex; i++) {
    if (!m_testFilter ||
        strstr(Cases::s_names[i], m_testFilter) == Cases::s_names[i]) {
      metadata.totalCases++;
    }
    if (m_untilFilter &&
        strstr(Cases::s_names[i], m_untilFilter) == Cases::s_names[i]) {
      metadata.endIndex = i;
      break;
    }
  }
  return metadata;
}

}  // namespace Quiz

using namespace Quiz;

void ion_main(int argc, const char* const argv[]) {
  volatile int stakTop;
  Ion::setStackStart((void*)&stakTop);

  Cases::Sort();
  Runner::s_instance.parseArguments(argc, argv);
  Runner::s_instance.run();
}
