#ifndef POINCARE_EXAM_MODE_H
#define POINCARE_EXAM_MODE_H

#include <ion/persisting_bytes.h>

namespace Poincare {

class ExamMode {
  friend class Preferences;

 public:
  enum class Mode : uint8_t {
    Off = 0,
    Standard,
    Dutch,
    IBTest,
    PressToTest,
    Portuguese,
    English,
    NumberOfModes,
    Uninitialized = 0b1111,
  };

  union PressToTestFlags {
    bool operator==(const PressToTestFlags& other) const {
      return value == other.value;
    }

    uint16_t value;
    struct {
      bool forbidEquationSolver : 1;
      bool forbidInequalityGraphing : 1;
      bool forbidImplicitPlots : 1;
      bool forbidStatsDiagnostics : 1;
      bool forbidVectors : 1;
      bool forbidBasedLogarithm : 1;
      bool forbidSum : 1;
      bool forbidExactResults : 1;
      bool forbidElementsApp : 1;
    };
  };

  ExamMode() : m_mode(Mode::Uninitialized), m_flags(0) {}
  ExamMode(Mode mode, PressToTestFlags flags = {.value = 0})
      : m_mode(mode), m_flags(flags.value) { /* TODO assertions */
  }

  bool operator==(const ExamMode& other) const {
    return m_mode == other.m_mode && m_flags == other.m_flags;
  }

  Mode mode() const { return m_mode; }
  PressToTestFlags flags() const { return {.value = m_flags}; }
  bool isUninitialized() const { return m_mode == Mode::Uninitialized; }
  bool isActive() const { return !isUninitialized() && m_mode != Mode::Off; }
  bool forbidEquationSolver() const { return flags().forbidEquationSolver; }
  bool forbidInequalityGraphing() const {
    return flags().forbidInequalityGraphing;
  }
  bool forbidImplicitPlots() const {
    return flags().forbidImplicitPlots || mode() == Mode::IBTest;
  }
  bool forbidStatsDiagnostics() const { return flags().forbidStatsDiagnostics; }
  bool forbidVectorProduct() const {
    return flags().forbidVectors || mode() == Mode::IBTest;
  }
  bool forbidVectorNorm() const { return flags().forbidVectors; }
  bool forbidBasedLogarithm() const { return flags().forbidBasedLogarithm; }
  bool forbidSum() const { return flags().forbidSum; }
  bool forbidExactResults() const {
    return flags().forbidExactResults || mode() == Mode::Dutch;
  }
  bool forbidElementsApp() const { return flags().forbidElementsApp; }

 private:
  static ExamMode GetFromPersistingBytes() { return ExamMode(); }  // TODO

  void setInPersistingBytes() const {}  // TODO

  Mode m_mode : 4;
  uint16_t m_flags : 12;
};

static_assert(sizeof(ExamMode) ==
                  sizeof(Ion::PersistingBytes::PersistingBytesInt),
              "ExamMode should have the same size");

}  // namespace Poincare

#endif
