#pragma once

typedef void (*QuizCase)(void);

extern int quiz_number_of_cases;
extern QuizCase quiz_cases[];
extern const char* quiz_case_names[];
