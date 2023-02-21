// SPDX-License-Identifier: CC-BY-NC-ND-4.0
// Caution: Dutch exam mode is subject to a compliance certification by a
// government agency. Distribution of a non-certified Dutch exam mode is
// illegal.

#include <poincare/exam_mode.h>

namespace Poincare {

// Exam mode permissions

bool ExamMode::forbidSolverApp() const { return flags().forbidEquationSolver; }

bool ExamMode::forbidElementsApp() const {
  return flags().forbidElementsApp || ruleset() == Ruleset::Dutch ||
         ruleset() == Ruleset::Portuguese || ruleset() == Ruleset::English;
}

bool ExamMode::forbidCodeApp() const { return ruleset() == Ruleset::Dutch; }

bool ExamMode::forbidLineDetails() const {
  return ruleset() == Ruleset::IBTest;
}

bool ExamMode::forbidInequalityGraphing() const {
  return flags().forbidInequalityGraphing;
}

bool ExamMode::forbidImplicitPlots() const {
  return flags().forbidImplicitPlots || ruleset() == Ruleset::IBTest;
}

bool ExamMode::forbidStatsDiagnostics() const {
  return flags().forbidStatsDiagnostics;
}

bool ExamMode::forbidVectorProduct() const {
  return flags().forbidVectors || ruleset() == Ruleset::IBTest;
}

bool ExamMode::forbidVectorNorm() const { return flags().forbidVectors; }

bool ExamMode::forbidBasedLogarithm() const {
  return flags().forbidBasedLogarithm;
}

bool ExamMode::forbidSum() const { return flags().forbidSum; }

bool ExamMode::forbidUnits() const {
  return ruleset() == Ruleset::Dutch || ruleset() == Ruleset::IBTest;
}

bool ExamMode::forbidAdditionalResults() const {
  return ruleset() == Ruleset::Dutch || ruleset() == Ruleset::IBTest;
}

bool ExamMode::forbidExactResults() const {
  return flags().forbidExactResults || ruleset() == Ruleset::Dutch;
}

}  // namespace Poincare
