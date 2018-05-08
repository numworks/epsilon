#ifndef POINCARE_STATIC_HIERARCHY_H
#define POINCARE_STATIC_HIERARCHY_H

#include <poincare/expression.h>
#include <poincare/list_data.h>

namespace Poincare {

template<int T>
class StaticHierarchy : public Expression {
public:
  StaticHierarchy();
  StaticHierarchy(const Expression * const * operands, bool cloneOperands = true);
  StaticHierarchy(const Expression * expression, bool cloneOperands = true); // Specialized constructor for StaticHierarchy<1>
  StaticHierarchy(const Expression * expression1, const Expression * expression2, bool cloneOperands = true); // Specialized constructor for StaticHierarchy<2>
  ~StaticHierarchy();
  StaticHierarchy(const StaticHierarchy & other) = delete;
  StaticHierarchy(StaticHierarchy && other) = delete;
  StaticHierarchy& operator=(const StaticHierarchy & other) = delete;
  StaticHierarchy& operator=(StaticHierarchy && other) = delete;

  virtual void setArgument(ListData * listData, int numberOfEntries, bool clone);
  int numberOfOperands() const override { return T; }
  const Expression * const * operands() const override { return m_operands; }
  virtual bool hasValidNumberOfOperands(int numberOfOperands) const;
protected:
  void build(const Expression * const * operands, int numberOfOperands, bool cloneOperands);
  int simplificationOrderSameType(const Expression * e, bool canBeInterrupted) const override;
  const Expression * m_operands[T];
};

}

#endif
