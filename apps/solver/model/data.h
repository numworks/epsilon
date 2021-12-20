#ifndef SOLVER_MODELS_DATA_H
#define SOLVER_MODELS_DATA_H

#include <stdint.h>

namespace Solver {

enum class SimpleInterestUnknown : uint8_t {
  None = 0,
  n,
  rPct,
  P,
  I
};

class SimpleInterestData {
public:
  SimpleInterestUnknown m_unknown;
  double m_n;
  double m_rPct;
  double m_P;
  double m_I;
  bool m_yearConventionIs360;
};

enum class CompoundInterestUnknown : uint8_t {
  None = 0,
  N,
  rPct,
  PV,
  Pmt,
  FV
};

class CompoundInterestData {
public:
  CompoundInterestUnknown m_unknown;
  double m_N;
  double m_rPct;
  double m_PV;
  double m_Pmt;
  double m_FV;
  double m_PY;
  double m_CY;
  bool m_paymentIsEnd;
};

union FinanceDataUnion {
  SimpleInterestData m_simpleInterestData;
  CompoundInterestData m_compoundInterestData;
};

class FinanceData {
public:
  bool isSimpleInterest;
  FinanceDataUnion m_data;
};

}  // namespace Solver

#endif /* SOLVER_MODELS_DATA_H */
