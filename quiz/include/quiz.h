#ifndef QUIZ_H
#define QUIZ_H

#ifdef __cplusplus
#define QUIZ_CASE(name) extern "C" { void quiz_case_##name();}; void quiz_case_##name()
#else
#define QUIZ_CASE(name) void quiz_case_##name()
#endif

#endif
