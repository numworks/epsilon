#include "runner_helpers.h"

void flushGlobalData() { flushGlobalDataNoPool(); }

void exception_run(void (*inner_main)(const char*, const char*, const char*),
                   const char* testFilter, const char* fromFilter,
                   const char* untilFilter) {
  inner_main(testFilter, fromFilter, untilFilter);
}
