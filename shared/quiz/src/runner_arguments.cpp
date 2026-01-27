#include <cstring>

#include "runner.h"

namespace Quiz {

void Runner::parseArguments(int argc, const char* const argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--until") == 0) {
      assert(i + 1 < argc);
      m_untilFilter = argv[i + 1];
    } else if (strcmp(argv[i], "--from") == 0) {
      assert(i + 1 < argc);
      m_fromFilter = argv[i + 1];
    } else if (strcmp(argv[i], "--filter") == 0 || strcmp(argv[i], "-f") == 0) {
      assert(i + 1 < argc);
      m_testFilter = argv[i + 1];
    } else if (strcmp(argv[i], "--skip-assertions") == 0 ||
               strcmp(argv[i], "-s") == 0) {
      m_skipAssertions = true;
    } else if (strcmp(argv[i], "--chunk-id") == 0) {
      assert(i + 1 < argc);
      m_chunkId = atoi(argv[i + 1]);
    } else if (strcmp(argv[i], "--number-of-chunks") == 0) {
      assert(i + 1 < argc);
      m_numberOfChunks = atoi(argv[i + 1]);
    }
  }
}

}  // namespace Quiz
