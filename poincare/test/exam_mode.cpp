#include <ion/include/ion/persisting_bytes.h>
#include <poincare/exam_mode.h>
#include <quiz.h>

using namespace Poincare;

void assert_exam_mode(ExamMode::Ruleset ruleset) {
  ExamMode mode(ruleset);
  switch (ruleset) {
    case ExamMode::Ruleset::Off:
      quiz_assert(!mode.forbidSolverApp());
      quiz_assert(!mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(!mode.forbidGraphDetails());
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
      break;
    case ExamMode::Ruleset::Standard:
      quiz_assert(!mode.forbidSolverApp());
      quiz_assert(!mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(!mode.forbidGraphDetails());
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
      break;
    case ExamMode::Ruleset::Dutch:
      quiz_assert(!mode.forbidSolverApp());
      quiz_assert(mode.forbidElementsApp());
      quiz_assert(mode.forbidCodeApp());
      quiz_assert(!mode.forbidGraphDetails());
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
      break;
    case ExamMode::Ruleset::IBTest:
      quiz_assert(!mode.forbidSolverApp());
      quiz_assert(mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(mode.forbidGraphDetails());
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
      break;
    case ExamMode::Ruleset::Portuguese:
      quiz_assert(!mode.forbidSolverApp());
      quiz_assert(mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(!mode.forbidGraphDetails());
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
      break;
    case ExamMode::Ruleset::English:
      quiz_assert(!mode.forbidSolverApp());
      quiz_assert(mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(!mode.forbidGraphDetails());
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
      break;
    case ExamMode::Ruleset::STAAR:
      quiz_assert(!mode.forbidSolverApp());
      quiz_assert(!mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(!mode.forbidGraphDetails());
      quiz_assert(mode.forbidInequalityGraphing());
      quiz_assert(mode.forbidImplicitPlots());
      quiz_assert(!mode.forbidStatsDiagnostics());
      quiz_assert(!mode.forbidVectorProduct());
      quiz_assert(!mode.forbidVectorNorm());
      quiz_assert(!mode.forbidBasedLogarithm());
      quiz_assert(!mode.forbidSum());
      quiz_assert(!mode.forbidUnits());
      quiz_assert(!mode.forbidAdditionalResults());
      quiz_assert(!mode.forbidExactResults());
      break;
    case ExamMode::Ruleset::Keystone:
      quiz_assert(mode.forbidSolverApp());
      quiz_assert(!mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(mode.forbidGraphDetails());
      quiz_assert(!mode.forbidInequalityGraphing());
      quiz_assert(mode.forbidImplicitPlots());
      quiz_assert(!mode.forbidStatsDiagnostics());
      quiz_assert(!mode.forbidVectorProduct());
      quiz_assert(!mode.forbidVectorNorm());
      quiz_assert(!mode.forbidBasedLogarithm());
      quiz_assert(!mode.forbidSum());
      quiz_assert(!mode.forbidUnits());
      quiz_assert(!mode.forbidAdditionalResults());
      quiz_assert(mode.forbidExactResults());
      break;
    default:
      quiz_assert(ruleset == ExamMode::Ruleset::PressToTest);
      // PressToTest is asserted in another test.
      break;
  }
}

void assert_press_to_test() {
  {
    ExamMode mode(ExamMode::Ruleset::PressToTest,
                  {.forbidEquationSolver = true});
    quiz_assert(mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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
    ExamMode mode(ExamMode::Ruleset::PressToTest,
                  {.forbidInequalityGraphing = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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
    ExamMode mode(ExamMode::Ruleset::PressToTest,
                  {.forbidImplicitPlots = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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
    ExamMode mode(ExamMode::Ruleset::PressToTest,
                  {.forbidStatsDiagnostics = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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
    ExamMode mode(ExamMode::Ruleset::PressToTest, {.forbidVectors = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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
    ExamMode mode(ExamMode::Ruleset::PressToTest,
                  {.forbidBasedLogarithm = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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
    ExamMode mode(ExamMode::Ruleset::PressToTest, {.forbidSum = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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
    ExamMode mode(ExamMode::Ruleset::PressToTest, {.forbidExactResults = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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
    ExamMode mode(ExamMode::Ruleset::PressToTest, {.forbidElementsApp = true});
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(!mode.forbidGraphDetails());
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

QUIZ_CASE(poincare_exam_modes) {
  for (int i = 0; i < static_cast<int>(ExamMode::Ruleset::NumberOfRulesets);
       i++) {
    assert_exam_mode(static_cast<ExamMode::Ruleset>(i));
  }
  assert_press_to_test();
}
