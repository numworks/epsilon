#ifndef QUIZ_H
#define QUIZ_H

#include <assert.h>
#include <quiz/src/symbols.h>
#include <stdbool.h>

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
  void quiz_case_##name();                                             \
  RegisterQuizCase register_quiz_case_##name(#name, quiz_case_##name); \
  void quiz_case_##name()

void quiz_assert(bool condition);
void quiz_print(const char* message);
extern bool sSkipAssertions;

#endif
