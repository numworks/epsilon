#include <ion/include/ion/persisting_bytes.h>
#include <poincare/exam_mode.h>
#include <quiz.h>

using namespace Poincare;

QUIZ_CASE(poincare_exam_mode_off) {
  ExamMode mode(ExamMode::Mode::Off);
  quiz_assert(!mode.forbidSolverApp());
  quiz_assert(!mode.forbidElementsApp());
  quiz_assert(!mode.forbidCodeApp());
  quiz_assert(!mode.forbidLineDetails());
  quiz_assert(!mode.forbidInequalityGraphing());
  quiz_assert(!mode.forbidImplicitPlots());
  quiz_assert(!mode.forbidStatsDiagnostics());
  quiz_assert(!mode.forbidVectorProduct());
  quiz_assert(!mode.forbidVectorNorm());
  quiz_assert(!mode.forbidBasedLogarithm());
  quiz_assert(!mode.forbidSum());
  quiz_assert(!mode.forbidUnits());
  quiz_assert(!mode.forbidAdditionalResults());
  quiz_assert(!mode.forbidExactResults());
}

QUIZ_CASE(poincare_exam_mode_standard) {
  ExamMode mode(ExamMode::Mode::Standard);
  quiz_assert(!mode.forbidSolverApp());
  quiz_assert(!mode.forbidElementsApp());
  quiz_assert(!mode.forbidCodeApp());
  quiz_assert(!mode.forbidLineDetails());
  quiz_assert(!mode.forbidInequalityGraphing());
  quiz_assert(!mode.forbidImplicitPlots());
  quiz_assert(!mode.forbidStatsDiagnostics());
  quiz_assert(!mode.forbidVectorProduct());
  quiz_assert(!mode.forbidVectorNorm());
  quiz_assert(!mode.forbidBasedLogarithm());
  quiz_assert(!mode.forbidSum());
  quiz_assert(!mode.forbidUnits());
  quiz_assert(!mode.forbidAdditionalResults());
  quiz_assert(!mode.forbidExactResults());
}

QUIZ_CASE(poincare_exam_mode_dutch) {
  ExamMode mode(ExamMode::Mode::Dutch);
  quiz_assert(!mode.forbidSolverApp());
  quiz_assert(mode.forbidElementsApp());
  quiz_assert(mode.forbidCodeApp());
  quiz_assert(!mode.forbidLineDetails());
  quiz_assert(!mode.forbidInequalityGraphing());
  quiz_assert(!mode.forbidImplicitPlots());
  quiz_assert(!mode.forbidStatsDiagnostics());
  quiz_assert(!mode.forbidVectorProduct());
  quiz_assert(!mode.forbidVectorNorm());
  quiz_assert(!mode.forbidBasedLogarithm());
  quiz_assert(!mode.forbidSum());
  quiz_assert(mode.forbidUnits());
  quiz_assert(mode.forbidAdditionalResults());
  quiz_assert(mode.forbidExactResults());
}

QUIZ_CASE(poincare_exam_mode_ibtest) {
  ExamMode mode(ExamMode::Mode::IBTest);
  quiz_assert(!mode.forbidSolverApp());
  quiz_assert(!mode.forbidElementsApp());
  quiz_assert(!mode.forbidCodeApp());
  quiz_assert(mode.forbidLineDetails());
  quiz_assert(!mode.forbidInequalityGraphing());
  quiz_assert(mode.forbidImplicitPlots());
  quiz_assert(!mode.forbidStatsDiagnostics());
  quiz_assert(mode.forbidVectorProduct());
  quiz_assert(!mode.forbidVectorNorm());
  quiz_assert(!mode.forbidBasedLogarithm());
  quiz_assert(!mode.forbidSum());
  quiz_assert(mode.forbidUnits());
  quiz_assert(mode.forbidAdditionalResults());
  quiz_assert(!mode.forbidExactResults());
}

QUIZ_CASE(poincare_exam_mode_portuguese) {
  ExamMode mode(ExamMode::Mode::Portuguese);
  quiz_assert(!mode.forbidSolverApp());
  quiz_assert(mode.forbidElementsApp());
  quiz_assert(!mode.forbidCodeApp());
  quiz_assert(!mode.forbidLineDetails());
  quiz_assert(!mode.forbidInequalityGraphing());
  quiz_assert(!mode.forbidImplicitPlots());
  quiz_assert(!mode.forbidStatsDiagnostics());
  quiz_assert(!mode.forbidVectorProduct());
  quiz_assert(!mode.forbidVectorNorm());
  quiz_assert(!mode.forbidBasedLogarithm());
  quiz_assert(!mode.forbidSum());
  quiz_assert(!mode.forbidUnits());
  quiz_assert(!mode.forbidAdditionalResults());
  quiz_assert(!mode.forbidExactResults());
}

QUIZ_CASE(poincare_exam_mode_english) {
  ExamMode mode(ExamMode::Mode::English);
  quiz_assert(!mode.forbidSolverApp());
  quiz_assert(mode.forbidElementsApp());
  quiz_assert(!mode.forbidCodeApp());
  quiz_assert(!mode.forbidLineDetails());
  quiz_assert(!mode.forbidInequalityGraphing());
  quiz_assert(!mode.forbidImplicitPlots());
  quiz_assert(!mode.forbidStatsDiagnostics());
  quiz_assert(!mode.forbidVectorProduct());
  quiz_assert(!mode.forbidVectorNorm());
  quiz_assert(!mode.forbidBasedLogarithm());
  quiz_assert(!mode.forbidSum());
  quiz_assert(!mode.forbidUnits());
  quiz_assert(!mode.forbidAdditionalResults());
  quiz_assert(!mode.forbidExactResults());
}

QUIZ_CASE(poincare_exam_mode_press_to_test) {
  {
    ExamMode mode(ExamMode::Mode::PressToTest, {.forbidEquationSolver = true});
    quiz_assert(mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(!mode.forbidInequalityGraphing());
    quiz_assert(!mode.forbidImplicitPlots());
    quiz_assert(!mode.forbidStatsDiagnostics());
    quiz_assert(!mode.forbidVectorProduct());
    quiz_assert(!mode.forbidVectorNorm());
    quiz_assert(!mode.forbidBasedLogarithm());
    quiz_assert(!mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(!mode.forbidExactResults());
  }
  {
    ExamMode mode(ExamMode::Mode::PressToTest,
                  {.forbidInequalityGraphing = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(mode.forbidInequalityGraphing());
    quiz_assert(!mode.forbidImplicitPlots());
    quiz_assert(!mode.forbidStatsDiagnostics());
    quiz_assert(!mode.forbidVectorProduct());
    quiz_assert(!mode.forbidVectorNorm());
    quiz_assert(!mode.forbidBasedLogarithm());
    quiz_assert(!mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(!mode.forbidExactResults());
  }
  {
    ExamMode mode(ExamMode::Mode::PressToTest, {.forbidImplicitPlots = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(!mode.forbidInequalityGraphing());
    quiz_assert(mode.forbidImplicitPlots());
    quiz_assert(!mode.forbidStatsDiagnostics());
    quiz_assert(!mode.forbidVectorProduct());
    quiz_assert(!mode.forbidVectorNorm());
    quiz_assert(!mode.forbidBasedLogarithm());
    quiz_assert(!mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(!mode.forbidExactResults());
  }
  {
    ExamMode mode(ExamMode::Mode::PressToTest,
                  {.forbidStatsDiagnostics = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(!mode.forbidInequalityGraphing());
    quiz_assert(!mode.forbidImplicitPlots());
    quiz_assert(mode.forbidStatsDiagnostics());
    quiz_assert(!mode.forbidVectorProduct());
    quiz_assert(!mode.forbidVectorNorm());
    quiz_assert(!mode.forbidBasedLogarithm());
    quiz_assert(!mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(!mode.forbidExactResults());
  }
  {
    ExamMode mode(ExamMode::Mode::PressToTest, {.forbidVectors = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(!mode.forbidInequalityGraphing());
    quiz_assert(!mode.forbidImplicitPlots());
    quiz_assert(!mode.forbidStatsDiagnostics());
    quiz_assert(mode.forbidVectorProduct());
    quiz_assert(mode.forbidVectorNorm());
    quiz_assert(!mode.forbidBasedLogarithm());
    quiz_assert(!mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(!mode.forbidExactResults());
  }
  {
    ExamMode mode(ExamMode::Mode::PressToTest, {.forbidBasedLogarithm = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(!mode.forbidInequalityGraphing());
    quiz_assert(!mode.forbidImplicitPlots());
    quiz_assert(!mode.forbidStatsDiagnostics());
    quiz_assert(!mode.forbidVectorProduct());
    quiz_assert(!mode.forbidVectorNorm());
    quiz_assert(mode.forbidBasedLogarithm());
    quiz_assert(!mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(!mode.forbidExactResults());
  }
  {
    ExamMode mode(ExamMode::Mode::PressToTest, {.forbidSum = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(!mode.forbidInequalityGraphing());
    quiz_assert(!mode.forbidImplicitPlots());
    quiz_assert(!mode.forbidStatsDiagnostics());
    quiz_assert(!mode.forbidVectorProduct());
    quiz_assert(!mode.forbidVectorNorm());
    quiz_assert(!mode.forbidBasedLogarithm());
    quiz_assert(mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(!mode.forbidExactResults());
  }
  {
    ExamMode mode(ExamMode::Mode::PressToTest, {.forbidExactResults = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(!mode.forbidInequalityGraphing());
    quiz_assert(!mode.forbidImplicitPlots());
    quiz_assert(!mode.forbidStatsDiagnostics());
    quiz_assert(!mode.forbidVectorProduct());
    quiz_assert(!mode.forbidVectorNorm());
    quiz_assert(!mode.forbidBasedLogarithm());
    quiz_assert(!mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(mode.forbidExactResults());
  }
  {
    ExamMode mode(ExamMode::Mode::PressToTest, {.forbidElementsApp = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidLineDetails());
    quiz_assert(!mode.forbidInequalityGraphing());
    quiz_assert(!mode.forbidImplicitPlots());
    quiz_assert(!mode.forbidStatsDiagnostics());
    quiz_assert(!mode.forbidVectorProduct());
    quiz_assert(!mode.forbidVectorNorm());
    quiz_assert(!mode.forbidBasedLogarithm());
    quiz_assert(!mode.forbidSum());
    quiz_assert(!mode.forbidUnits());
    quiz_assert(!mode.forbidAdditionalResults());
    quiz_assert(!mode.forbidExactResults());
  }
}
