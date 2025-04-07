#include "runner_helpers.h"

void flushGlobalData() { flushGlobalDataNoPool(); }

void exception_run(void (*inner_main)(const char*, const char*, const char*,
                                      int, int),
                   const char* testFilter, const char* fromFilter,
                   const char* untilFilter, int chunkId, int numberOfChunks) {
  inner_main(testFilter, fromFilter, untilFilter, chunkId, numberOfChunks);
}
