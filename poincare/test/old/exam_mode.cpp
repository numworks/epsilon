#include <poincare/exam_mode.h>
#include <quiz.h>

using namespace Poincare;
using PTTFlags = ExamMode::PressToTestFlags::Flags;

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
      quiz_assert(!mode.forbidSimultaneousEquationSolver());
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
      quiz_assert(!mode.forbidSimultaneousEquationSolver());
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
      quiz_assert(!mode.forbidSimultaneousEquationSolver());
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
      quiz_assert(!mode.forbidSimultaneousEquationSolver());
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
      quiz_assert(!mode.forbidSimultaneousEquationSolver());
      break;
    case ExamMode::Ruleset::English:
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
      quiz_assert(!mode.forbidUnits());
      quiz_assert(!mode.forbidAdditionalResults());
      quiz_assert(!mode.forbidExactResults());
      quiz_assert(!mode.forbidSimultaneousEquationSolver());
      break;
    case ExamMode::Ruleset::STAAR:
      quiz_assert(!mode.forbidSolverApp());
      quiz_assert(!mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(mode.forbidGraphDetails());
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
      quiz_assert(mode.forbidSimultaneousEquationSolver());
      break;
    case ExamMode::Ruleset::Pennsylvania:
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
      quiz_assert(!mode.forbidSimultaneousEquationSolver());
      break;
    case ExamMode::Ruleset::SouthCarolina:
    case ExamMode::Ruleset::NorthCarolina:
      quiz_assert(mode.forbidSolverApp());
      quiz_assert(!mode.forbidElementsApp());
      quiz_assert(!mode.forbidCodeApp());
      quiz_assert(mode.forbidGraphDetails());
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
      quiz_assert(!mode.forbidSimultaneousEquationSolver());
      break;
    default:
      quiz_assert(ruleset == ExamMode::Ruleset::PressToTest);
      // PressToTest is asserted in another test.
      break;
  }
}

void assert_press_to_test() {
  {
    ExamMode mode(
        ExamMode::Ruleset::PressToTest,
        ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidEquationSolver));
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
                  ExamMode::PressToTestFlags().setFlag(
                      PTTFlags::ForbidInequalityGraphing));
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
    ExamMode mode(
        ExamMode::Ruleset::PressToTest,
        ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidImplicitPlots));
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
    ExamMode mode(
        ExamMode::Ruleset::PressToTest,
        ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidGraphDetails));
    quiz_assert(!mode.forbidSolverApp());
    quiz_assert(!mode.forbidElementsApp());
    quiz_assert(!mode.forbidCodeApp());
    quiz_assert(mode.forbidGraphDetails());
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
    ExamMode mode(
        ExamMode::Ruleset::PressToTest,
        ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidStatsDiagnostics));
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
    ExamMode mode(
        ExamMode::Ruleset::PressToTest,
        ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidVectors));
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
    ExamMode mode(
        ExamMode::Ruleset::PressToTest,
        ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidBasedLogarithm));
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
    ExamMode mode(ExamMode::Ruleset::PressToTest,
                  ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidSum));
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
    ExamMode mode(
        ExamMode::Ruleset::PressToTest,
        ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidExactResults));
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
    ExamMode mode(
        ExamMode::Ruleset::PressToTest,
        ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidElementsApp));
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
  for (Ion::ExamBytes::Int i = 0; i < Ion::ExamMode::k_numberOfModes; i++) {
    assert_exam_mode(static_cast<ExamMode::Ruleset>(i));
  }
  assert_press_to_test();
}
