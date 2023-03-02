// SPDX-License-Identifier: CC-BY-NC-ND-4.0
// Caution: Dutch exam mode is subject to a compliance certification by a
// government agency. Distribution of a non-certified Dutch exam mode is
// illegal.

#ifndef POINCARE_EXAM_MODE_H
#define POINCARE_EXAM_MODE_H

#include <assert.h>
#include <ion/exam_mode.h>

namespace Poincare {

class ExamMode : public Ion::ExamMode::Configuration {
 public:
  using Ruleset = Ion::ExamMode::Ruleset;

  struct PressToTestFlags {
    bool operator==(const PressToTestFlags& other) const {
      assert(unusedBits == 0 && discardedBits == 0);
      assert(other.unusedBits == 0 && other.discardedBits == 0);
      return PressToTestUnion{.flags = *this}.value ==
             PressToTestUnion{.flags = other}.value;
    }

    bool forbidEquationSolver : 1;
    bool forbidInequalityGraphing : 1;
    bool forbidImplicitPlots : 1;
    bool forbidStatsDiagnostics : 1;
    bool forbidVectors : 1;
    bool forbidBasedLogarithm : 1;
    bool forbidSum : 1;
    bool forbidExactResults : 1;
    bool forbidElementsApp : 1;
    // These bits are available, unused and preserved at 0 for == operator
    Ion::ExamMode::Int unusedBits : 5;
    // These bits will be discarded when cast into 14 bits in an Configuration
    Ion::ExamMode::Int discardedBits : 2;
  };
  static_assert(sizeof(PressToTestFlags) <= sizeof(Ion::ExamMode::Int));

  ExamMode() : Configuration() {}
  explicit ExamMode(Ruleset rules, PressToTestFlags flags = {})
      : Configuration(rules, PressToTestUnion{.flags = flags}.value) {}
  ExamMode(Configuration config) : Configuration(config) {}

  PressToTestFlags flags() const {
    return PressToTestUnion{.value = Configuration::flags()}.flags;
  }

  // Exam mode permissions
  bool forbidSolverApp() const {
    return flags().forbidEquationSolver || ruleset() == Ruleset::Keystone;
  }
  bool forbidElementsApp() const {
    return flags().forbidElementsApp || ruleset() == Ruleset::Dutch ||
           ruleset() == Ruleset::Portuguese || ruleset() == Ruleset::English;
  }
  bool forbidCodeApp() const { return ruleset() == Ruleset::Dutch; }
  bool forbidLineDetails() const { return ruleset() == Ruleset::IBTest; }
  bool forbidInequalityGraphing() const {
    return flags().forbidInequalityGraphing || ruleset() == Ruleset::STAAR;
  }
  bool forbidImplicitPlots() const {
    return flags().forbidImplicitPlots || ruleset() == Ruleset::IBTest ||
           ruleset() == Ruleset::Keystone || ruleset() == Ruleset::STAAR;
  }
  bool forbidStatsDiagnostics() const { return flags().forbidStatsDiagnostics; }
  bool forbidVectorProduct() const {
    return flags().forbidVectors || ruleset() == Ruleset::IBTest;
  }
  bool forbidVectorNorm() const { return flags().forbidVectors; }
  bool forbidBasedLogarithm() const { return flags().forbidBasedLogarithm; }
  bool forbidSum() const { return flags().forbidSum; }
  bool forbidUnits() const {
    return ruleset() == Ruleset::Dutch || ruleset() == Ruleset::IBTest;
  }
  bool forbidAdditionalResults() const {
    return ruleset() == Ruleset::Dutch || ruleset() == Ruleset::IBTest;
  }
  bool forbidExactResults() const {
    return flags().forbidExactResults || ruleset() == Ruleset::Dutch ||
           ruleset() == Ruleset::Keystone;
  }

 private:
  union PressToTestUnion {
    Ion::ExamMode::Int value;
    PressToTestFlags flags;
  };
};

static_assert(sizeof(ExamMode) == sizeof(Ion::ExamMode::Configuration),
              "ExamMode size is not compatible with PersistingBytes");

}  // namespace Poincare

#endif
