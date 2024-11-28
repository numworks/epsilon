/* Caution: Dutch exam mode is subject to a compliance certification by a
 * government agency. Distribution of a non-certified Dutch exam mode is
 * illegal. */

#ifndef POINCARE_EXAM_MODE_H
#define POINCARE_EXAM_MODE_H

#include <assert.h>
#include <ion/exam_mode.h>
#include <omg/code_guard.h>

namespace Poincare {

class __attribute__((packed)) ExamMode : public Ion::ExamMode::Configuration {
 public:
  using Ruleset = Ion::ExamMode::Ruleset;

  struct PressToTestFlags {
    CODE_GUARD(press_to_test_flags, 2156906052,  //
               enum class Flags
               : size_t{
                   ForbidExactResults = 0,
                   ForbidEquationSolver,
                   ForbidInequalityGraphing,
                   ForbidImplicitPlots,
                   ForbidGraphDetails,
                   ForbidElementsApp,
                   ForbidStatsDiagnostics,
                   ForbidVectors,
                   ForbidBasedLogarithm,
                   ForbidSum,
                   NumberOfFlags,
               };)
    static_assert(static_cast<size_t>(Flags::NumberOfFlags) <=
                  Ion::ExamMode::Configuration::k_dataSize);

    bool operator==(const PressToTestFlags& other) const {
      return m_bits == other.m_bits;
    }

    bool getFlag(Flags flag) const {
      assert(static_cast<size_t>(flag) <
             static_cast<size_t>(Flags::NumberOfFlags));
      return OMG::BitHelper::bitAtIndex(m_bits, flag);
    }
    PressToTestFlags& setFlag(Flags flag, bool value = true) {
      assert(static_cast<size_t>(flag) <
             static_cast<size_t>(Flags::NumberOfFlags));
      /* Do not use setBitAtIndex as the template drops attributes, and
       * Emscripten does not like taking an align(2) reference to an align(1)
       * value. */
      m_bits = OMG::BitHelper::withBitsBetweenIndexes(
          m_bits, static_cast<size_t>(flag), static_cast<size_t>(flag),
          static_cast<Ion::ExamMode::Int>(value));
      return *this;
    }

    Ion::ExamMode::Int m_bits;
  };
  static_assert(sizeof(PressToTestFlags) == sizeof(Ion::ExamMode::Int));

  ExamMode() : Configuration() {}
  explicit ExamMode(Ruleset rules, PressToTestFlags flags = {})
      : Configuration(rules, PressToTestUnion{.flags = flags}.value) {}
  ExamMode(Configuration config) : Configuration(config) {}

  PressToTestFlags flags() const {
    return PressToTestUnion{.value = Configuration::flags()}.flags;
  }

  using Flags = PressToTestFlags::Flags;

  // Exam mode permissions
  bool forbidSolverApp() const {
    return flags().getFlag(Flags::ForbidEquationSolver) ||
           ruleset() == Ruleset::Pennsylvania ||
           ruleset() == Ruleset::SouthCarolina ||
           ruleset() == Ruleset::NorthCarolina;
  }
  bool forbidElementsApp() const {
    return flags().getFlag(Flags::ForbidElementsApp) ||
           ruleset() == Ruleset::Dutch || ruleset() == Ruleset::Portuguese ||
           ruleset() == Ruleset::English || ruleset() == Ruleset::IBTest;
  }
  bool forbidCodeApp() const {
    return ruleset() == Ruleset::Dutch || ruleset() == Ruleset::English;
  }
  bool forbidGraphDetails() const {
    return flags().getFlag(Flags::ForbidGraphDetails) ||
           ruleset() == Ruleset::IBTest || ruleset() == Ruleset::Pennsylvania ||
           ruleset() == Ruleset::STAAR || ruleset() == Ruleset::SouthCarolina ||
           ruleset() == Ruleset::NorthCarolina;
  }
  bool forbidInequalityGraphing() const {
    return flags().getFlag(Flags::ForbidInequalityGraphing) ||
           ruleset() == Ruleset::STAAR;
  }
  bool forbidImplicitPlots() const {
    return flags().getFlag(Flags::ForbidImplicitPlots) ||
           ruleset() == Ruleset::IBTest || ruleset() == Ruleset::Pennsylvania ||
           ruleset() == Ruleset::STAAR;
  }
  bool forbidStatsDiagnostics() const {
    return flags().getFlag(Flags::ForbidStatsDiagnostics);
  }
  bool forbidVectorProduct() const {
    return flags().getFlag(Flags::ForbidVectors) ||
           ruleset() == Ruleset::IBTest;
  }
  bool forbidVectorNorm() const {
    return flags().getFlag(Flags::ForbidVectors);
  }
  bool forbidBasedLogarithm() const {
    return flags().getFlag(Flags::ForbidBasedLogarithm);
  }
  bool forbidSum() const { return flags().getFlag(Flags::ForbidSum); }
  bool forbidUnits() const {
    return ruleset() == Ruleset::Dutch || ruleset() == Ruleset::IBTest;
  }
  bool forbidAdditionalResults() const {
    return ruleset() == Ruleset::Dutch || ruleset() == Ruleset::IBTest;
  }
  bool forbidExactResults() const {
    return flags().getFlag(Flags::ForbidExactResults) ||
           ruleset() == Ruleset::Dutch || ruleset() == Ruleset::Pennsylvania ||
           ruleset() == Ruleset::SouthCarolina ||
           ruleset() == Ruleset::NorthCarolina;
  }
  bool forbidSimultaneousEquationSolver() const {
    return ruleset() == Ruleset::STAAR;
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
