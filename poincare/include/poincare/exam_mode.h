#ifndef POINCARE_EXAM_MODE_H
#define POINCARE_EXAM_MODE_H

#include <assert.h>
#include <ion/persisting_bytes.h>
#include <kandinsky/color.h>

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
      : m_mode(mode), m_flags(flags.value) {
    assert(isValid());
  }

  bool operator==(const ExamMode& other) const {
    return m_mode == other.m_mode && m_flags == other.m_flags;
  }

  Mode mode() const { return m_mode; }
  PressToTestFlags flags() const { return {.value = m_flags}; }
  bool isUninitialized() const { return m_mode == Mode::Uninitialized; }
  bool isActive() const { return !isUninitialized() && m_mode != Mode::Off; }
  KDColor color() const;

  // Exam mode permissions
  bool forbidSolverApp() const;
  bool forbidElementsApp() const;
  bool forbidCodeApp() const;
  bool forbidLineDetails() const;
  bool forbidInequalityGraphing() const;
  bool forbidImplicitPlots() const;
  bool forbidStatsDiagnostics() const;
  bool forbidVectorProduct() const;
  bool forbidVectorNorm() const;
  bool forbidBasedLogarithm() const;
  bool forbidSum() const;
  bool forbidUnits() const;
  bool forbidAdditionalResults() const;
  bool forbidExactResults() const;

 private:
  static ExamMode GetFromPersistingBytes();

  bool isValid() const {
    return (m_flags == 0 || m_mode == Mode::PressToTest) &&
           (m_mode < Mode::NumberOfModes);
  }
  void setInPersistingBytes() const;

  Mode m_mode : 4;
  uint16_t m_flags : 12;
};

static_assert(sizeof(ExamMode) ==
                  sizeof(Ion::PersistingBytes::PersistingBytesInt),
              "ExamMode should have the same size");

}  // namespace Poincare

#endif
