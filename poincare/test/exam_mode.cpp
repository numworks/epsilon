#include <ion/include/ion/persisting_bytes.h>
#include <poincare/preferences.h>
#include <quiz.h>

using namespace Poincare;

void assert_exam_mode_is(Ion::PersistingBytes::PersistingBytesInt bytes,
                         Preferences::ExamMode mode) {
  Preferences::ExamPersistingBytes exam(bytes);
  quiz_assert(exam.mode() == mode);
}

void assert_exam_params_are(Ion::PersistingBytes::PersistingBytesInt bytes,
                            Preferences::PressToTestParams params) {
  Preferences::ExamPersistingBytes exam(bytes);
  quiz_assert(params == Preferences::k_inactivePressToTest ||
              exam.mode() == Preferences::ExamMode::PressToTest);
  quiz_assert(exam.params() == params);
}

QUIZ_CASE(exam_mode) {
  assert_exam_mode_is(0xffff, Preferences::ExamMode::Unknown);
  Preferences::PressToTestParams params = Preferences::PressToTestParams({0});
  params.m_equationSolverIsForbidden = 1;
  params.m_inequalityGraphingIsForbidden = 1;
  params.m_implicitPlotsAreForbidden = 1;
  params.m_statsDiagnosticsAreForbidden = 1;
  params.m_vectorsAreForbidden = 1;
  params.m_basedLogarithmIsForbidden = 1;
  params.m_sumIsForbidden = 1;
  params.m_exactResultsAreForbidden = 1;
  params.m_elementsAppIsForbidden = 1;
  assert_exam_params_are(0x1ff4, params);
  assert_exam_mode_is(0x001, Preferences::ExamMode::Standard);
}
