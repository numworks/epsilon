#include <ion.h>
#include <poincare/print.h>

#include "quiz.h"
#include "runner_helpers.h"
#include "symbols.h"

void quiz_print(const char* message) { Ion::Console::writeLine(message); }

bool quiz_print_clear() { return Ion::Console::clear(); }

/* [testFilter] if set: only run tests starting with [testFilter]
 *
 * [fromFilter]
 * if set: run all tests located after any test starting with [fromFilter]
 *
 * [untilFilter]
 * if set: run all tests but quits when any test start with [untilFilter]
 *
 * Those 3 conditions can be mixed and matched to obtain the desired test filter
 * Example:
 * `--from aaa --filter bbb`
 *  => run tests starting with bbb located after aaa
 * `--from aaa --until bbb`
 *  => run tests located inbetween aaa and bbb
 * `--from aaa --until bbb --filter ccc`
 *  => run tests starting with ccc located inbetween aaa and bbb
 *
 * [chunk-id and number-of-chunks]
 * Split the list of tests into [number-of-chunks] chunks and run only the tests
 * of the [chunk-id] chunk. These flags are used to parallelize the test run
 * among several processes.
 */
static inline void ion_main_inner(const char* testFilter,
                                  const char* fromFilter,
                                  const char* untilFilter, int chunkId,
                                  int numberOfChunks) {
  int i = 0;
  int time = Ion::Timing::millis();
  int totalCases = 0;
#ifndef PLATFORM_DEVICE
  bool afterFrom = fromFilter == nullptr;
#endif

  // First pass to count the number of quiz cases
  while (quiz_cases[i] != NULL) {
#ifndef PLATFORM_DEVICE
    if (testFilter &&
        strstr(quiz_case_names[i], testFilter) != quiz_case_names[i]) {
      i++;
      continue;
    }
#endif
    i++;
    totalCases++;
  }

  int start = 0;
  int end = totalCases;
  if (numberOfChunks != 1) {
    int chunkSize = (totalCases + numberOfChunks - 1) / numberOfChunks;
    start = chunkId * chunkSize;
    end = std::min(start + chunkSize, end);
  }

  // Second pass to test quiz cases
  constexpr int k_bufferSize = 30;
  char buffer[k_bufferSize];
  i = 0;
  int caseIndex = 0;
  while (quiz_cases[i] != NULL) {
#ifndef PLATFORM_DEVICE
    if (caseIndex >= end) {
      break;
    }
    if (fromFilter &&
        strstr(quiz_case_names[i], fromFilter) == quiz_case_names[i]) {
      afterFrom = true;
    }
    if (!afterFrom) {
      i++;
      continue;
    }
    if (untilFilter &&
        strstr(quiz_case_names[i], untilFilter) == quiz_case_names[i]) {
      break;
    }
    if (testFilter &&
        strstr(quiz_case_names[i], testFilter) != quiz_case_names[i]) {
      i++;
      continue;
    }
#if ASAN
    /* When ASAN=1, "python_numpy" reaches an assert due to an error in python
     * "RuntimeError: maximum recursion depth exceeded" */
    // TODO: pass this test even when ASAN=1
    if (strcmp(quiz_case_names[i], "python_numpy") == 0) {
      i++;
      continue;
    }
#endif
    if (caseIndex < start) {
      i++;
      caseIndex++;
      continue;
    }
#endif
    caseIndex++;
    QuizCase c = quiz_cases[i];
    if (quiz_print_clear()) {
      // Avoid cluttering the display if it can't be cleared
      Poincare::Print::CustomPrintf(buffer, k_bufferSize, "TEST: %i/%i",
                                    caseIndex, totalCases);
      quiz_print(buffer);
    }
    quiz_print(quiz_case_names[i]);
    flushGlobalData();
    c();
    i++;
  }
  flushGlobalData();

  quiz_print_clear();

  if (numberOfChunks != 1) {
    return;
  }

  // Display test results
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "ALL %i TESTS FINISHED",
                                end - start);
  quiz_print(buffer);

  // Display test duration
  time = Ion::Timing::millis() - time;
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "DURATION: %i ms", time);
  quiz_print(buffer);
#ifdef PLATFORM_DEVICE
  while (1) {
    Ion::Timing::msleep(100000);
  }
#endif
}

void ion_main(int argc, const char* const argv[]) {
  const char* testFilter = nullptr;
  const char* fromFilter = nullptr;
  const char* untilFilter = nullptr;
  int chunkId = 0;
  int numberOfChunks = 1;
  sSkipAssertions = false;
#if !PLATFORM_DEVICE
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--until") == 0) {
      assert(i + 1 < argc);
      untilFilter = argv[i + 1];
    } else if (strcmp(argv[i], "--from") == 0) {
      assert(i + 1 < argc);
      fromFilter = argv[i + 1];
    } else if (strcmp(argv[i], "--filter") == 0 || strcmp(argv[i], "-f") == 0) {
      assert(i + 1 < argc);
      testFilter = argv[i + 1];
    } else if (strcmp(argv[i], "--skip-assertions") == 0 ||
               strcmp(argv[i], "-s") == 0) {
      sSkipAssertions = true;
    } else if (strcmp(argv[i], "--chunk-id") == 0) {
      assert(i + 1 < argc);
      chunkId = atoi(argv[i + 1]);
    } else if (strcmp(argv[i], "--number-of-chunks") == 0) {
      assert(i + 1 < argc);
      numberOfChunks = atoi(argv[i + 1]);
    }
  }
#endif
  assert(chunkId < numberOfChunks);
  /* s_stackStart must be defined as early as possible to ensure that there
   * cannot be allocated memory pointers before. Otherwise, with MicroPython for
   * example, stack pointer could go backward after initialization and allocated
   * memory pointers could be overlooked during mark procedure. */
  init();
  volatile int stackTop;
  Ion::setStackStart((void*)(&stackTop));
  exception_run(ion_main_inner, testFilter, fromFilter, untilFilter, chunkId,
                numberOfChunks);
  shutdown();
}
