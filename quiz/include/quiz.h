#ifndef QUIZ_H
#define QUIZ_H

#include <stdbool.h>

#ifdef __cplusplus
#define QUIZ_CASE(name) extern "C" { void quiz_case_##name();}; void quiz_case_##name()
#else
#define QUIZ_CASE(name) void quiz_case_##name()
#endif

#ifdef __cplusplus
extern "C" {
#endif

void quiz_assert_true(bool condition);
void quiz_print(const char * message);

#ifdef __cplusplus
}
#endif

#endif
