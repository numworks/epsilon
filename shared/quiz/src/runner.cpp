#include "runner.h"

#include <ion.h>
#include <omg/unreachable.h>
#include <poincare/print.h>
#include <stdlib.h>

#include "cases.h"

static bool clearConsole() { return Ion::Console::clear(); }

namespace Quiz {

OMG::GlobalBox<Runner> Runner::s_instance;

void Runner::innerRun() {
  assert(0 <= m_chunkId && m_chunkId < m_numberOfChunks);

  constexpr int k_bufferSize = 64;
  char buffer[k_bufferSize];
  TestsMetadata metadata = computeTestsMetadata();

  int t0 = Ion::Timing::millis();
  int caseIndex = 0;
  for (int i = metadata.startIndex; i < metadata.endIndex; i++) {
    if (m_testFilter && !testMatchesFilter(Cases::s_names[i], m_testFilter)) {
      continue;
    }
#if ASAN
    /* When ASAN=1, "python_numpy" reaches an assert due to an error in python
     * "RuntimeError: maximum recursion depth exceeded" */
    // TODO: pass this test even when ASAN=1
    if (strcmp(testName, "python_numpy") == 0) {
      continue;
    }
#endif
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
      if (testMatchesFilter(Cases::s_names[i], m_fromFilter)) {
        fromIndex = i;
        break;
      }
    }
    metadata.startIndex = fromIndex;
  }
  for (int i = metadata.startIndex; i < metadata.endIndex; i++) {
    if (!m_testFilter || testMatchesFilter(Cases::s_names[i], m_testFilter)) {
      metadata.totalCases++;
    }
    if (m_untilFilter && testMatchesFilter(Cases::s_names[i], m_untilFilter)) {
      metadata.endIndex = i;
      break;
    }
  }
  return metadata;
}

bool Runner::testMatchesFilter(const char* testName, const char* filter) const {
#if PLATFORM_DEVICE
  OMG_UNREACHABLE;
#else
  assert(filter != nullptr);
  return strstr(testName, filter) == testName;
#endif
}

}  // namespace Quiz

using namespace Quiz;

void ion_main(int argc, const char* const argv[]) {
  volatile int stackTop;
  Ion::setStackStart((void*)&stackTop);

  Cases::Sort();
  Runner::s_instance.init();
  Runner::s_instance->parseArguments(argc, argv);
  Runner::s_instance->run();
  exit(0);
}
