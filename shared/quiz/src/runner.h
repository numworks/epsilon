#pragma once

#include <assert.h>
#include <quiz.h>

namespace Quiz {

class Runner {
 public:
  static Runner s_instance;

  Runner() = default;

  void parseArguments(int argc, const char* const argv[]);

  void run() {
    Env::Init();
    exceptionRun();
    Env::Shutdown();
  }

  bool skipAssertions() const { return m_skipAssertions; }

 private:
  void exceptionRun();

  void innerRun();

  struct TestsMetadata {
    int startIndex;
    int endIndex;
    int totalCases;
  };
  TestsMetadata computeTestsMetadata();

  /* [testFilter] if set: only run tests starting with [testFilter]
   *
   * [fromFilter]
   * if set: run all tests located after any test starting with [fromFilter]
   *
   * [untilFilter]
   * if set: run all tests but quits when any test start with [untilFilter]
   *
   * Those 3 conditions can be mixed and matched to obtain the desired test
   * filter Example:
   * `--from aaa --filter bbb`
   *  => run tests starting with bbb located after aaa
   * `--from aaa --until bbb`
   *  => run tests located inbetween aaa and bbb
   * `--from aaa --until bbb --filter ccc`
   *  => run tests starting with ccc located inbetween aaa and bbb
   *
   * [chunk-id and number-of-chunks]
   * Split the list of tests into [number-of-chunks] chunks and run only the
   * tests of the [chunk-id] chunk. These flags are used to parallelize the test
   * run among several processes.
   */
  const char* m_testFilter = nullptr;
  const char* m_fromFilter = nullptr;
  const char* m_untilFilter = nullptr;
  int m_chunkId = 0;
  int m_numberOfChunks = 1;
  bool m_skipAssertions = false;
};

}  // namespace Quiz
