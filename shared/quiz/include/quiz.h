#ifndef QUIZ_H
#define QUIZ_H

#include <assert.h>
#include <quiz/src/symbols.h>
#include <stdbool.h>

#ifdef __cplusplus

/* Each quiz case has a associated static instance of RegisterQuizCase that adds
 * quiz_case to a static array. */
struct RegisterQuizCase {
  RegisterQuizCase(const char* name, QuizCase function) {
    // Note that the array is initialized in fiasco order
    // assert(quiz_number_of_cases < k_maxNumberOfCases);
    quiz_case_names[quiz_number_of_cases] = name;
    quiz_cases[quiz_number_of_cases++] = function;
  };
};

#define QUIZ_CASE(name)                                                \
  extern "C" {                                                         \
  void quiz_case_##name();                                             \
  };                                                                   \
  RegisterQuizCase register_quiz_case_##name(#name, quiz_case_##name); \
  void quiz_case_##name()
#else
#define QUIZ_CASE(name) void quiz_case_##name()
#endif

#ifdef __cplusplus
extern "C" {
#endif

void quiz_assert(bool condition);
void quiz_print(const char* message);
extern bool sSkipAssertions;

#ifdef __cplusplus
}
#endif

#endif
